//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_LEX_STATIC_LEXER_FEB_10_2008_0753PM)
#define BOOST_SPIRIT_LEX_STATIC_LEXER_FEB_10_2008_0753PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/support/safe_bool.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/token.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/static_functor.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/iterator.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/unique_id.hpp>
#if defined(BOOST_SPIRIT_DEBUG)
#include <boost/spirit/home/support/detail/lexer/debug.hpp>
#endif
#include <boost/algorithm/string/predicate.hpp>

namespace boost { namespace spirit { namespace lex { namespace lexertl
{ 
    ///////////////////////////////////////////////////////////////////////////
    //  forward declaration
    ///////////////////////////////////////////////////////////////////////////
    namespace static_
    {
        struct lexer;
    }

    ///////////////////////////////////////////////////////////////////////////
    //  static_token_set
    ///////////////////////////////////////////////////////////////////////////
    template <typename Token
      , typename LexerTables = static_::lexer
      , typename Iterator = typename Token::iterator_type>
    class static_token_set
    {
    protected:
        typedef typename boost::detail::iterator_traits<Iterator>::value_type 
            char_type;
        typedef std::basic_string<char_type> string_type;
        typedef LexerTables tables_type;

        static std::size_t get_state_id(char const* state)
        {
            for (std::size_t i = 0; i < tables_type::state_count(); ++i)
            {
                if (boost::equals(tables_type::state_name(i), state))
                    return i;
            }
            return ~0;
        }

    public:
        typedef Token token_type;
        typedef typename Token::id_type id_type;

        static_token_set(unsigned int flags = 0) {}

        // interface for token definition management
        std::size_t add_token (char_type const* state
          , string_type const& tokendef, std::size_t token_id) 
        {
            return unique_id<id_type>::get();
        }

        // interface for pattern definition management
        void add_pattern (char_type const* state, string_type const& name
          , string_type const& patterndef) {}

        void clear() {}

        std::size_t add_state(char_type const* state)
        {
            return get_state_id(state);
        }
        string_type initial_state() const 
        { 
            return tables_type::state_name(0);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  Every lexer type to be used as a lexer for Spirit has to conform to 
    //  the following public interface:
    //
    //    typedefs: 
    //        iterator_type   The type of the iterator exposed by this lexer.
    //        token_type      The type of the tokens returned from the exposed 
    //                        iterators.
    //        token_set       The type of the token set representing a lexer 
    //                        state.
    //
    //    functions:
    //        default constructor
    //                        Since lexers are instantiated as base classes 
    //                        only it might be a good idea to make this 
    //                        constructor protected.
    //        begin, end      Return a pair of iterators, when dereferenced
    //                        returning the sequence of tokens recognized in 
    //                        the input stream given as the parameters to the 
    //                        begin() function.
    //        add_token       Should add the definition of a token to be 
    //                        recognized by this lexer.
    //        clear           Should delete all current token definitions
    //                        associated with the given state of this lexer 
    //                        object.
    //
    //    template parameters:
    //        Token           The type of the tokens to be returned from the
    //                        exposed token iterator.
    //        LexerTables     See explanations below.
    //        Iterator        The type of the iterator used to access the
    //                        underlying character stream.
    //        Functor         The type of the InputPolicy to use to instantiate
    //                        the multi_pass iterator type to be used as the 
    //                        token iterator (returned from begin()/end()).
    //        TokenSet        The type of the token set to use in conjunction 
    //                        with this lexer type. This is used for the 
    //                        token_set typedef described above only.
    //
    //    Additionally, this implementation of a static lexer has a template
    //    parameter LexerTables allowing to customize the static lexer tables
    //    to be used. The LexerTables is expected to be a type exposing 
    //    the following functions:
    //
    //        static std::size_t const state_count()
    //
    //                This function needs toreturn the number of lexer states
    //                contained in the table returned from the state_names()
    //                function.
    //
    //        static char const* const* state_names()
    //
    //                This function needs to return a pointer to a table of
    //                names of all lexer states. The table needs to have as 
    //                much entries as the state_count() function returns
    //
    //        template<typename Iterator>
    //        std::size_t next(std::size_t &start_state_, Iterator const& start_
    //          , Iterator &start_token_, Iterator const& end_
    //          , std::size_t& unique_id_);
    //
    //                This function is expected to return the next matched
    //                token from the underlying input stream.
    //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    //
    //  The static_lexer class is a implementation of a Spirit.Lex 
    //  lexer on top of Ben Hanson's lexertl library (For more information 
    //  about lexertl go here: http://www.benhanson.net/lexertl.html). 
    //
    //  This class is designed to be used in conjunction with a generated, 
    //  static lexer. For more information see the documentation (The Static 
    //  Lexer Model).
    //
    //  This class is supposed to be used as the first and only template 
    //  parameter while instantiating instances of a lex::lexer class.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename Token = token<>
      , typename LexerTables = static_::lexer
      , typename Iterator = typename Token::iterator_type
      , typename Functor = static_functor<Token, Iterator, mpl::false_>
      , typename TokenSet = 
            lex::token_set<static_token_set<Token, LexerTables, Iterator> > >
    class static_lexer 
    {
    public:
        // operator_bool() is needed for the safe_bool base class
        operator typename safe_bool<static_lexer>::result_type() const 
        { 
            return safe_bool<static_lexer>()(true);    // object is always valid
        }

        typedef typename boost::detail::iterator_traits<Iterator>::value_type 
            char_type;
        typedef std::basic_string<char_type> string_type;

        //  Every lexer type to be used as a lexer for Spirit has to conform to 
        //  a public interface 
        typedef Token token_type;
        typedef typename Token::id_type id_type;
        typedef TokenSet token_set;
        typedef iterator<Functor> iterator_type;

    private:
        // this type is purely used for the iterator_type construction below
        struct iterator_data_type 
        {
            typename Functor::next_token_functor next_;
            typename Functor::semantic_actions_type const& actions_;
            std::size_t (*get_state_id_)(char const*);
        };

        typedef LexerTables tables_type;

        static std::size_t get_state_id(char const* state)
        {
            for (std::size_t i = 0; i < tables_type::state_count(); ++i)
            {
                if (boost::equals(tables_type::state_name(i), state))
                    return i;
            }
            return ~0;
        }

    public:
        //  Return the start iterator usable for iterating over the generated
        //  tokens, the generated function next_token(...) is called to match 
        //  the next token from the input.
        template <typename Iterator_>
        iterator_type begin(Iterator_& first, Iterator_ const& last) const
        { 
            iterator_data_type iterator_data = { 
                    &tables_type::template next<Iterator_>, actions_, 
                    get_state_id 
                };
            return iterator_type(iterator_data, first, last);
        }

        //  Return the end iterator usable to stop iterating over the generated 
        //  tokens.
        iterator_type end() const
        { 
            return iterator_type(); 
        }

    protected:
        //  Lexer instances can be created by means of a derived class only.
        static_lexer(unsigned int flags) {}

    public:
        // interface for token definition management
        std::size_t add_token (char_type const* state, char_type tokendef
          , std::size_t token_id) 
        {
            return unique_id<id_type>::get();
        }
        std::size_t add_token (char_type const* state, string_type const& tokendef
          , std::size_t token_id) 
        {
            return unique_id<id_type>::get();
        }
        std::size_t add_token(char_type const* state, token_set& tokset) 
        {
            return unique_id<id_type>::get();
        }

        // interface for pattern definition management
        void add_pattern (char_type const* state, string_type const& name
          , string_type const& patterndef) {}

        void clear(char_type const* state) {}

        std::size_t add_state(char_type const* state)
        {
            return get_state_id(state);
        }
        string_type initial_state() const 
        { 
            return tables_type::state_name(0);
        }

        // register a semantic action with the given id
        template <typename F>
        void add_action(id_type unique_id, std::size_t state, F act) 
        {
            // If you get compilation errors below stating value_type not being
            // a member of boost::fusion::unused_type, then you are probably
            // using semantic actions in your token definition without 
            // the static_actor_lexer being specified as the base class of your 
            // lexer (instead of the static_lexer class).
            typedef typename Functor::semantic_actions_type::value_type
                value_type;
            typedef typename Functor::wrap_action_type wrapper_type;

            if (actions_.size() <= state)
                actions_.resize(state + 1); 

            value_type& actions (actions_[state]);
            if (actions.size() <= unique_id)
                actions.resize(unique_id + 1); 

            actions[unique_id] = wrapper_type::call(act);
        }

        bool init_dfa() const { return true; }

    private:
        typename Functor::semantic_actions_type actions_;
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  The static_actor_lexer class is another implementation of a 
    //  Spirit.Lex lexer on top of Ben Hanson's lexertl library as outlined 
    //  above (For more information about lexertl go here: 
    //  http://www.benhanson.net/lexertl.html).
    //
    //  Just as the static_lexer class it is meant to be used with 
    //  a statically generated lexer as outlined above.
    //
    //  The only difference to the static_lexer class above is that 
    //  token_def definitions may have semantic (lexer) actions attached while 
    //  being defined:
    //
    //      int w;
    //      token_def<> word = "[^ \t\n]+";
    //      self = word[++ref(w)];        // see example: word_count_lexer
    //
    //  This class is supposed to be used as the first and only template 
    //  parameter while instantiating instances of a lex::lexer class.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename Token = token<>
      , typename LexerTables = static_::lexer
      , typename Iterator = typename Token::iterator_type
      , typename Functor = static_functor<Token, Iterator, mpl::true_>
      , typename TokenSet = 
            lex::token_set<static_token_set<Token, LexerTables, Iterator> > >
    class static_actor_lexer 
      : public static_lexer<Token, LexerTables, Iterator, Functor, TokenSet>
    {
    protected:
        // Lexer instances can be created by means of a derived class only.
        static_actor_lexer(unsigned int flags) 
          : static_lexer<Token, LexerTables, Iterator, Functor, TokenSet>(flags) 
        {}
    };

}}}}

#endif
