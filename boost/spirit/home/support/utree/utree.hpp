/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011      Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_UTREE)
#define BOOST_SPIRIT_UTREE

#include <cstddef>
#include <algorithm>
#include <string>
#include <ostream>
#include <typeinfo>

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_polymorphic.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/ref.hpp>

#include <boost/spirit/home/support/utree/detail/utree_detail1.hpp>

#if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable: 4804)
# pragma warning(disable: 4805)
# pragma warning(disable: 4244)
#endif

namespace boost { namespace spirit
{
    //[utree_exceptions
    /*` All exceptions thrown by utree are derived from utree_exception. */
    struct utree_exception : std::exception {};

    /*`The `bad_type_exception` is thrown whenever somebody calls a member 
       function, which applies to certain stored utree_type's only, but this 
       precondition is violated as the `utree` instance holds some other type.
    */
    struct bad_type_exception : utree_exception
    {
        virtual const char* what() const throw()
        {
            return "utree: Illegal operation for currently stored data.";
        }
    };
    //]

    //[utree_types
    /*`Each instance of an `utree` data structure can store exactly one of the 
       following data types at a time: 
    */
    struct utree_type
    {
        enum info
        {
            nil_type,           // nil is the sentinel (empty) utree type.
            list_type,          // A doubly linked list of utrees.
            range_type,         // A range of list::iterators. 
            reference_type,     // A reference to another utree.
            any_type,           // A pointer or reference to any C++ type. 
            function_type,      // A utree holding a stored_function<F> object,
                                // where F is an unary function object taking a 
                                // scope as it's parameter and returning a
                                // utree.

            // numeric atoms
            bool_type,          // An utree holding a boolean value
            int_type,           // An utree holding a integer (int) value
            double_type,        // An utree holding a floating point (double) value

            // text atoms (utf8)
            string_type,        // An UTF-8 string 
            string_range_type,  // A pair of iterators into an UTF-8 string
            symbol_type,        // An UTF-8 symbol name

            binary_type         // Arbitrary binary data
        };
    };
    //]

    ///////////////////////////////////////////////////////////////////////////
    // The nil type
    ///////////////////////////////////////////////////////////////////////////
    struct nil {};

    ///////////////////////////////////////////////////////////////////////////
    // A typed string with parametric Base storage. The storage can be any
    // range or (stl container) of chars.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Base, utree_type::info type_>
    struct basic_string : Base
    {
        static utree_type::info const type = type_;

        basic_string()
          : Base() {}

        basic_string(Base const& base)
          : Base(base) {}

        template <typename Iterator>
        basic_string(Iterator bits, std::size_t len)
          : Base(bits, bits + len) {}

        template <typename Iterator>
        basic_string(Iterator first, Iterator last)
          : Base(first, last) {}

        basic_string& operator=(basic_string const& other)
        {
            Base::operator=(other);
            return *this;
        }

        basic_string& operator=(Base const& other)
        {
            Base::operator=(other);
            return *this;
        }
    };

    //[utree_strings
    /*`The `utree` string types described below are used by the `utree` API 
       only. These are not used to store information in the `utree` itself.
       Their purpose is to refer to different internal `utree` node types
       only. For instance, creating a `utree` from a binary data type will 
       create a `binary_type` utree node (see above).
    */
    /*`The binary data type can be represented either verbatim as a sequence 
       of bytes or as a pair of iterators into some other stored binary data 
       sequence. Use this string type to access/create a `binary_type` `utree`.
    */
    typedef basic_string<
        boost::iterator_range<char const*>, utree_type::binary_type
    > binary_range_type;
    typedef basic_string<
        std::string, utree_type::binary_type
    > binary_string_type;

    /*`The UTF-8 string can be represented either verbatim as a sequence of 
       characters or as a pair of iterators into some other stored binary data 
       sequence. Use this string type to access/create a `string_type` `utree`.
    */
    typedef basic_string<
        boost::iterator_range<char const*>, utree_type::string_type
    > utf8_string_range_type;
    typedef basic_string<
        std::string, utree_type::string_type
    > utf8_string_type;

    /*`The UTF-8 symbol can be represented either verbatim as a sequence of 
       characters or as a pair of iterators into some other stored binary data 
       sequence. Use this string type to access/create a `symbol_type` `utree`.
    */
    typedef basic_string<
        boost::iterator_range<char const*>, utree_type::symbol_type
    > utf8_symbol_range_type;
    typedef basic_string<
        std::string, utree_type::symbol_type
    > utf8_symbol_type;
    //]

    ///////////////////////////////////////////////////////////////////////////
    // Our function type
    ///////////////////////////////////////////////////////////////////////////
    class utree;
    class scope;

    //[utree_function_object_interface
    struct function_base
    {
        virtual ~function_base() {};
        virtual utree operator()(scope const& env) const = 0;
        virtual function_base* clone() const = 0; // Calling f.clone() must
                                                  // return a newly allocated
                                                  // function_base instance
                                                  // that is equal to f.
    };

    template <typename F>
    struct stored_function : function_base
    {
        F f;
        stored_function(F f = F());
        virtual ~stored_function();
        virtual utree operator()(scope const& env) const;
        virtual function_base* clone() const;
    };
    //]

    ///////////////////////////////////////////////////////////////////////////
    // Shallow tag. Instructs utree to hold an iterator_range
    // as-is without deep copying the range.
    ///////////////////////////////////////////////////////////////////////////
    struct shallow_tag {};
    shallow_tag const shallow = {};

    ///////////////////////////////////////////////////////////////////////////
    // A void* plus type_info
    ///////////////////////////////////////////////////////////////////////////
    class any_ptr
    {
    public:
        template <typename Ptr>
        typename boost::disable_if<
            boost::is_polymorphic<
                typename boost::remove_pointer<Ptr>::type>,
            Ptr>::type
        get() const
        {
            if (*i == typeid(Ptr))
            {
                return static_cast<Ptr>(p);
            }
            throw std::bad_cast();
        }

        template <typename T>
        any_ptr(T* p)
          : p(p), i(&typeid(T*))
        {}

        friend bool operator==(any_ptr const& a, any_ptr const& b)
        {
            return (a.p == b.p) && (*a.i == *b.i);
        }

    private:
        // constructor is private
        any_ptr(void* p, std::type_info const* i)
          : p(p), i(i) {}

        template <typename UTreeX, typename UTreeY>
        friend struct detail::visit_impl;

        friend class utree;

        void* p;
        std::type_info const* i;
    };

    //[utree
    class utree {
    public:
        //[utree_container_types
        typedef utree value_type;
        typedef utree& reference;
        typedef utree const& const_reference;
        typedef std::ptrdiff_t difference_type;
        typedef std::size_t size_type;

        typedef detail::list::node_iterator<utree> iterator;
        typedef detail::list::node_iterator<utree const> const_iterator;
        //]

        typedef detail::list::node_iterator<boost::reference_wrapper<utree> >
          ref_iterator;

        typedef boost::iterator_range<iterator> range;
        typedef boost::iterator_range<const_iterator> const_range;

        // dtor
        ~utree();

        ////////////////////////////////////////////////////////////////////////
        //[utree_initialization
        /*`A `utree` can be constructed or initialized from a wide range of 
           data types, allowing to create `utree` instances for every 
           possible node type (see the description of `utree_type::info` above). 
           For this reason it exposes a constructor and a assignment operator 
           for each of the allowed node types as shown below:
        */
        // constructs `nil_type` node
        utree();

        // initializes a `boolean_type` node
        utree(bool);
        reference operator=(bool);

        // initializes a `integer_type` node
        utree(unsigned int);
        utree(int);
        reference operator=(unsigned int);
        reference operator=(int);

        // initializes a `double_type` node
        utree(double);
        reference operator=(double);

        // initializes a `string_type` node
        utree(char);
        utree(char const*);
        utree(char const*, std::size_t);
        utree(std::string const&);
        reference operator=(char);
        reference operator=(char const*);
        reference operator=(std::string const&);

        // constructs a `string_range_type` node, does not copy the data
        // but stores the iterator range
        utree(utf8_string_range_type const&, shallow_tag);

        // initializes a `reference_type` node
        utree(boost::reference_wrapper<utree>);
        reference operator=(boost::reference_wrapper<utree>);

        // initializes an `any_type` node
        utree(any_ptr const&);
        reference operator=(any_ptr const&);

        // initialize a `range_type` node
        template <class Iterator>
        utree(boost::iterator_range<Iterator>);
        template <class Iterator>
        reference operator=(boost::iterator_range<Iterator>);

        // initialize a `function_type` node
        template <class F>
        utree(stored_function<F> const&);
        template <class F>
        reference operator=(stored_function<F> const&);

        // initialize either a `string_type`, a `symbol_type`, or a `binary_type`
        // node (depending on the template parameter `type_`)
        template <class Base, utree_type::info type_>
        utree(basic_string<Base, type_> const&);
        template <class Base, utree_type::info type_>
        reference operator=(basic_string<Base, type_> const&);
        //]

        // copy 
        utree(const_reference);
        reference operator=(const_reference);

        // range
        utree(range, shallow_tag);
        utree(const_range, shallow_tag);

        // assign dispatch
        template <class Iter>
        void assign(Iter, Iter);

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // function object visitation interface

        // single dispatch
        template <class F>
        typename boost::result_of<F(utree const&)>::type
        static visit(utree const&, F);

        template <class F>
        typename boost::result_of<F(utree&)>::type
        static visit(utree&, F);

        // double dispatch
        template <class F>
        typename boost::result_of<F(utree const&, utree const&)>::type
        static visit(utree const&, utree const&, F);

        template <class F>
        typename boost::result_of<F(utree&, utree const&)>::type
        static visit(utree&, utree const&, F);

        template <class F>
        typename boost::result_of<F(utree const&, utree&)>::type
        static visit(utree const&, utree&, F);

        template <class F>
        typename boost::result_of<F(utree&, utree&)>::type
        static visit(utree&, utree&, F);

        ////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////
        //[utree_container_functions
        // STL Container interface

        // insertion 
        template <class T>
        void push_back(T const&);
        template <class T>
        void push_front(T const&);
        template <class T>
        iterator insert(iterator, T const&);
        template <class T>
        void insert(iterator, std::size_t, T const&);
        template <class Iterator>
        void insert(iterator, Iterator, Iterator);

        // erasure
        void pop_front();
        void pop_back();
        iterator erase(iterator);
        iterator erase(iterator, iterator);

        // front access
        reference front();
        const_reference front() const;
        iterator begin();
        const_iterator begin() const;
        ref_iterator ref_begin();

        // back access
        reference back();
        const_reference back() const;
        iterator end();
        const_iterator end() const;
        ref_iterator ref_end();
        //]

        // random access
        reference operator[](size_type);
        const_reference operator[](size_type) const;

        void clear();

        void swap(utree&);

        bool empty() const;
        size_type size() const;

        ////////////////////////////////////////////////////////////////////////

        //[utree_variant_functions
        // return the data type (`utree_type::info`) of the currently stored 
        // data item
        utree_type::info which() const;

        // access the currently stored data in a type safe manner, this will 
        // throw a `std::bad_cast()` if the currently stored data item is not 
        // default convertible to `T`.
        template <class T>
        T get() const;
        //]

        reference deref();
        const_reference deref() const;

        short tag() const;
        void tag(short);

        utree eval(scope const&) const;

    //<-
    private:
        typedef utree_type type;

        template <class UTreeX, class UTreeY>
        friend struct detail::visit_impl;
        friend struct detail::index_impl;
        friend struct detail::assign_impl;

        template <class T>
        friend struct detail::get_impl;

        type::info get_type() const;
        void set_type(type::info);
        void ensure_list_type();
        void free();
        void copy(const_reference);

        union {
            detail::fast_string s;
            detail::list l;
            detail::range r;
            detail::string_range sr;
            detail::void_ptr v;
            bool b;
            int i;
            double d;
            utree* p;
            function_base* pf;
        };
    //->
    };
    //]

    //[utree_scope
    class scope : public boost::iterator_range<utree*> {
      public:
        scope(utree* first = 0,
              utree* last = 0,
              scope const* parent = 0) 
              //<-
              : boost::iterator_range<utree*>(first, last)
              , parent(parent)
              , depth(parent? parent->depth + 1 : 0) {}
              //->

        scope const* outer() const 
        //<-
        {
            return parent;
        }
        //->

        std::size_t level() const 
        //<-
        {
            return depth;
        }
        //->

      //<-  
      private:
        scope const* parent;
        std::size_t depth;
      //->
    };
    //]
}}

#if defined(BOOST_MSVC)
  #pragma warning(pop)
#endif

#endif

