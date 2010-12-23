/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c)      2010 Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/support_utree.hpp>

#include <iostream>
#include <sstream>
#include <cstdlib>

inline bool check(boost::spirit::utree const& val, std::string expected)
{
    std::stringstream s;
    s << val;
    if (s.str() == expected + " ")
        return true;

    std::cerr << "got result: " << s.str() 
              << ", expected: " << expected << std::endl;
    return false;
}

struct one_two_three
{
    boost::spirit::utree operator()(boost::spirit::scope) const
    {
        return boost::spirit::utree(123);
    }
};

int main()
{
    using boost::spirit::utree;
    using boost::spirit::nil;
    using boost::spirit::uninitialized;
    using boost::spirit::utf8_symbol_type;
    using boost::spirit::binary_string_type;

    {
        // test the size
        std::cout << "size of utree is: "
            << sizeof(utree) << " bytes" << std::endl;
        BOOST_TEST(sizeof(utree) == sizeof(void*[4]));
    }

    {
        utree val(nil);
        BOOST_TEST(check(val, "<nil>"));
    }

    {
        utree val(true);
        BOOST_TEST(check(val, "true"));
    }

    {
        utree val(123);
        BOOST_TEST(check(val, "123"));
    }

    {
        // single element string
        utree val('x');
        BOOST_TEST(check(val, "\"x\""));
        
        // empty string 
        utree val1("");
        BOOST_TEST(check(val1, "\"\""));
    }

    {
        utree val(123.456);
        BOOST_TEST(check(val, "123.456"));
    }

    { // strings
        utree val("Hello, World");
        BOOST_TEST(check(val, "\"Hello, World\""));
        utree val2;
        val2 = val;
        BOOST_TEST(check(val2, "\"Hello, World\""));
        utree val3("Hello, World. Chuckie is back!!!");
        val = val3;
        BOOST_TEST(check(val, "\"Hello, World. Chuckie is back!!!\""));

        utree val4("Apple");
        utree val5("Apple");
        BOOST_TEST(val4 == val5);

        utree val6("ApplePie");
        BOOST_TEST(val4 < val6);
    }

    { // symbols
        utree val(utf8_symbol_type("Hello, World"));
        BOOST_TEST(check(val, "Hello, World"));
        utree val2;
        val2 = val;
        BOOST_TEST(check(val2, "Hello, World"));
        utree val3(utf8_symbol_type("Hello, World. Chuckie is back!!!"));
        val = val3;
        BOOST_TEST(check(val, "Hello, World. Chuckie is back!!!"));

        utree val4(utf8_symbol_type("Apple"));
        utree val5(utf8_symbol_type("Apple"));
        BOOST_TEST(val4 == val5);

        utree val6(utf8_symbol_type("ApplePie"));
        BOOST_TEST(val4 < val6);
    }

    { // binary_strings
        utree val(binary_string_type("\xDE#\xAD"));
        BOOST_TEST(check(val, "#de23ad#" /* FIXME?: "#\xDE#\xAD#" */));
        utree val2;
        val2 = val;
        BOOST_TEST(check(val2, "#de23ad#" /* FIXME?: "#\xDE#\xAD#" */));
        utree val3(binary_string_type("\xDE\xAD\xBE\xEF"));
        val = val3;
        BOOST_TEST(check(val, "#deadbeef#" /* FIXME?: "#\xDE\xAD\xBE\xEF#" */));

        utree val4(binary_string_type("\x01"));
        utree val5(binary_string_type("\x01"));
        BOOST_TEST(val4 == val5);

        utree val6(binary_string_type("\x01\x02"));
        BOOST_TEST(val4 < val6);
    }

    {
        utree val;
        val.push_back(123);
        val.push_back("Chuckie");
        BOOST_TEST(val.size() == 2);
        utree val2;
        val2.push_back(123.456);
        val2.push_back("Mah Doggie");
        val.push_back(val2);
        BOOST_TEST(val.size() == 3);
        BOOST_TEST(check(val, "( 123 \"Chuckie\" ( 123.456 \"Mah Doggie\" ) )"));
        BOOST_TEST(check(val.front(), "123"));

        utree val3(nil);
        val3.swap(val);
        BOOST_TEST(val3.size() == 3);
        BOOST_TEST(check(val, "<nil>"));
        val3.swap(val);
        BOOST_TEST(check(val, "( 123 \"Chuckie\" ( 123.456 \"Mah Doggie\" ) )"));
        val.push_back("another string");
        BOOST_TEST(val.size() == 4);
        BOOST_TEST(check(val, "( 123 \"Chuckie\" ( 123.456 \"Mah Doggie\" ) \"another string\" )"));
        val.pop_front();
        BOOST_TEST(check(val, "( \"Chuckie\" ( 123.456 \"Mah Doggie\" ) \"another string\" )"));
        utree::iterator i = val.begin();
        ++++i;
        val.insert(i, "Right in the middle");
        BOOST_TEST(val.size() == 4);
        BOOST_TEST(check(val, "( \"Chuckie\" ( 123.456 \"Mah Doggie\" ) \"Right in the middle\" \"another string\" )"));
        val.pop_back();
        BOOST_TEST(check(val, "( \"Chuckie\" ( 123.456 \"Mah Doggie\" ) \"Right in the middle\" )"));
        BOOST_TEST(val.size() == 3);
        utree::iterator it = val.end(); --it;
        val.erase(it);
        BOOST_TEST(check(val, "( \"Chuckie\" ( 123.456 \"Mah Doggie\" ) )"));
        BOOST_TEST(val.size() == 2);

        val.insert(val.begin(), val2.begin(), val2.end());
        BOOST_TEST(check(val, "( 123.456 \"Mah Doggie\" \"Chuckie\" ( 123.456 \"Mah Doggie\" ) )"));
        BOOST_TEST(val.size() == 4);
    }

    {
        utree val;
        val.insert(val.end(), 123);
        val.insert(val.end(), "Mia");
        val.insert(val.end(), "Chuckie");
        val.insert(val.end(), "Poly");
        val.insert(val.end(), "Mochi");
        BOOST_TEST(check(val, "( 123 \"Mia\" \"Chuckie\" \"Poly\" \"Mochi\" )"));
    }

    {
        utree a(nil), b(nil);
        BOOST_TEST(a == b);
        a = 123;
        BOOST_TEST(a != b);
        b = 123;
        BOOST_TEST(a == b);
        a = 100.00;
        BOOST_TEST(a < b);

        b = a = utree(uninitialized);
        BOOST_TEST(a == b);
        a.push_back(1);
        a.push_back("two");
        a.push_back(3.0);
        b.push_back(1);
        b.push_back("two");
        b.push_back(3.0);
        BOOST_TEST(a == b);
        b.push_back(4);
        BOOST_TEST(a != b);
        BOOST_TEST(a < b);
    }

    {
        utree a(nil);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_back(4);
        a.push_back(5);
        a.push_back(6);
        a.push_back(7);
        a.push_back(8);
        a.push_back(9);
        a.push_back(10);
        a.push_back(11);
        a.push_back(12);

        BOOST_TEST(a[0] == utree(1));
        BOOST_TEST(a[1] == utree(2));
        BOOST_TEST(a[2] == utree(3));
        BOOST_TEST(a[3] == utree(4));
        BOOST_TEST(a[4] == utree(5));
        BOOST_TEST(a[5] == utree(6));
        BOOST_TEST(a[6] == utree(7));
        BOOST_TEST(a[7] == utree(8));
        BOOST_TEST(a[8] == utree(9));
        BOOST_TEST(a[9] == utree(10));
        BOOST_TEST(a[10] == utree(11));
        BOOST_TEST(a[11] == utree(12));
    }

    {
        // test empty list
        utree a;
        a.push_back(1);
        a.pop_front();
        BOOST_TEST(check(a, "( )"));

        // the other way around
        utree b;
        b.push_front(1);
        b.pop_back();
        BOOST_TEST(check(b, "( )"));
    }

    { // test references
        utree val(123);
        utree ref(boost::ref(val));
        BOOST_TEST(check(ref, "123"));
        BOOST_TEST(ref == utree(123));

        val.clear();
        val.push_back(1);
        val.push_back(2);
        val.push_back(3);
        val.push_back(4);
        BOOST_TEST(check(ref, "( 1 2 3 4 )"));
        BOOST_TEST(ref[0] == utree(1));
        BOOST_TEST(ref[1] == utree(2));
        BOOST_TEST(ref[2] == utree(3));
        BOOST_TEST(ref[3] == utree(4));
    }

    { // put it in an array

        utree vals[] = {
            utree(123),
            utree("Hello, World"),
            utree(123.456)
        };

        BOOST_TEST(check(vals[0], "123"));
        BOOST_TEST(check(vals[1], "\"Hello, World\""));
        BOOST_TEST(check(vals[2], "123.456"));
    }

    { // operators

        BOOST_TEST((utree(true) && utree(true)) == utree(true));
        BOOST_TEST((utree(true) || utree(false)) == utree(true));
        BOOST_TEST(!utree(true) == utree(false));

        BOOST_TEST((utree(456) + utree(123)) == utree(456 + 123));
        BOOST_TEST((utree(456) + utree(123.456)) == utree(456 + 123.456));
        BOOST_TEST((utree(456) - utree(123)) == utree(456 - 123));
        BOOST_TEST((utree(456) - utree(123.456)) == utree(456 - 123.456));
        BOOST_TEST((utree(456) * utree(123)) == utree(456 * 123));
        BOOST_TEST((utree(456) * utree(123.456)) == utree(456 * 123.456));
        BOOST_TEST((utree(456) / utree(123)) == utree(456 / 123));
        BOOST_TEST((utree(456) / utree(123.456)) == utree(456 / 123.456));
        BOOST_TEST((utree(456) % utree(123)) == utree(456 % 123));
        BOOST_TEST(-utree(456) == utree(-456));

        BOOST_TEST((utree(456) & utree(123)) == utree(456 & 123));
        BOOST_TEST((utree(456) | utree(123)) == utree(456 | 123));
        BOOST_TEST((utree(456) ^ utree(123)) == utree(456 ^ 123));
        BOOST_TEST((utree(456) << utree(3)) == utree(456 << 3));
        BOOST_TEST((utree(456) >> utree(2)) == utree(456 >> 2));
        BOOST_TEST(~utree(456) == utree(~456));
    }

    { // test reference iterator
        utree val;
        val.push_back(1);
        val.push_back(2);
        val.push_back(3);
        val.push_back(4);
        BOOST_TEST(check(val, "( 1 2 3 4 )"));

        utree::ref_iterator b = val.ref_begin();
        utree::ref_iterator e = val.ref_end();

        utree ref(boost::make_iterator_range(b, e));
        BOOST_TEST(ref[0] == utree(1));
        BOOST_TEST(ref[1] == utree(2));
        BOOST_TEST(ref[2] == utree(3));
        BOOST_TEST(ref[3] == utree(4));
        BOOST_TEST(check(ref, "( 1 2 3 4 )"));
    }

    {
        // check the tag
        utree x;
        x.tag(123);
        BOOST_TEST(x.tag() == 123);
    }

    {
        // test functions
        using boost::spirit::stored_function;
        using boost::spirit::scope;

        utree f = stored_function<one_two_three>();
        f.eval(scope());
    }

    {
        // shallow ranges
        using boost::spirit::shallow;

        utree val;
        val.push_back(1);
        val.push_back(2);
        val.push_back(3);
        val.push_back(4);

        utree::iterator i = val.begin(); ++i;
        utree alias(utree::range(i, val.end()), shallow);

        BOOST_TEST(check(alias, "( 2 3 4 )"));
        BOOST_TEST(alias.size() == 3);
        BOOST_TEST(alias.front() == 2);
        BOOST_TEST(alias.back() == 4);
        BOOST_TEST(!alias.empty());
        BOOST_TEST(alias[1] == 3);
    }

    {
        // shallow string ranges
        using boost::spirit::utf8_string_range_type;
        using boost::spirit::shallow;

        char const* s = "Hello, World";
        utree val(utf8_string_range_type(s, s + strlen(s)), shallow);
        BOOST_TEST(check(val, "\"Hello, World\""));

        utf8_string_range_type r = val.get<utf8_string_range_type>();
        utf8_string_range_type pf(r.begin()+1, r.end()-1);
        val = utree(pf, shallow);
        BOOST_TEST(check(val, "\"ello, Worl\""));
    }

    {
        // any pointer
        using boost::spirit::any_ptr;

        int n = 123;
        utree up = any_ptr(&n);
        BOOST_TEST(*up.get<int*>() == 123);
    }

    return boost::report_errors();
}
