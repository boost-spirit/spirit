//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/assign/std/vector.hpp>

#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_numeric.hpp>
#include <boost/spirit/include/karma_directive.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_action.hpp>
#include <boost/spirit/include/karma_nonterminal.hpp>
#include <boost/spirit/include/karma_auxiliary.hpp>
#include <boost/spirit/include/karma_phoenix_attributes.hpp>
#include <boost/spirit/include/support_argument.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "test.hpp"

using namespace spirit_test;

///////////////////////////////////////////////////////////////////////////////
struct action 
{
    action (std::vector<char>& vec) 
      : vec(vec), it(vec.begin()) 
    {}

    void operator()(unsigned& value, boost::spirit::unused_type, bool& pass) const
    {
       pass = (it != vec.end());
       if (pass)
           value = *it++;
    }

    std::vector<char>& vec;
    mutable std::vector<char>::iterator it;
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    using namespace boost::spirit::ascii;
    using boost::spirit::karma::repeat;
    using boost::spirit::karma::inf;
    using boost::spirit::karma::int_;
    using boost::spirit::karma::hex;
    using boost::spirit::karma::_1;

    {
        std::string str8("aaaaaaaa");
        BOOST_TEST(test("aaaaaaaa", repeat[char_], str8));   // kleene synonym
        BOOST_TEST(test("aaaaaaaa", repeat(8)[char_], str8));
        BOOST_TEST(test("aaa", repeat(3)[char_], str8));
        BOOST_TEST(!test("aaaaaaaa", repeat(9)[char_], str8));

        std::string str3("aaa");
        BOOST_TEST(test("aaaaa", repeat(3, 5)[char_], str8));
        BOOST_TEST(test("aaa", repeat(3, 5)[char_], str3));
        BOOST_TEST(!test("aaa", repeat(4, 5)[char_], str3));

        BOOST_TEST(test("aaa", repeat(3, inf)[char_], str3));
        BOOST_TEST(test("aaaaaaaa", repeat(3, inf)[char_], str8));
        BOOST_TEST(!test("aaa", repeat(4, inf)[char_], str3));
    }

    {
        std::string str8("aaaaaaaa");
        BOOST_TEST(test_delimited("a a a a a a a a ", repeat[char_], str8, space));
        BOOST_TEST(test_delimited("a a a a a a a a ", repeat(8)[char_], str8, space));
        BOOST_TEST(test_delimited("a a a ", repeat(3)[char_], str8, space));
        BOOST_TEST(!test_delimited("a a a a a a a a ", repeat(9)[char_], str8, space));

        std::string str3("aaa");
        BOOST_TEST(test_delimited("a a a a a ", repeat(3, 5)[char_], str8, space));
        BOOST_TEST(test_delimited("a a a ", repeat(3, 5)[char_], str3, space));
        BOOST_TEST(!test_delimited("a a a ", repeat(4, 5)[char_], str3, space));

        BOOST_TEST(test_delimited("a a a ", repeat(3, inf)[char_], str3, space));
        BOOST_TEST(test_delimited("a a a a a a a a ", repeat(3, inf)[char_], str8, space));
        BOOST_TEST(!test_delimited("a a a ", repeat(4, inf)[char_], str3, space));
    }

    { // lazy repeats
        using boost::phoenix::val;

        std::string str8("aaaaaaaa");
        BOOST_TEST(test("aaaaaaaa", repeat[char_], str8));   // kleene synonym
        BOOST_TEST(test("aaaaaaaa", repeat(val(8))[char_], str8));
        BOOST_TEST(test("aaa", repeat(val(3))[char_], str8));
        BOOST_TEST(!test("aaaaaaaa", repeat(val(9))[char_], str8));

        std::string str3("aaa");
        BOOST_TEST(test("aaaaa", repeat(val(3), val(5))[char_], str8));
        BOOST_TEST(test("aaa", repeat(val(3), val(5))[char_], str3));
        BOOST_TEST(!test("aaa", repeat(val(4), val(5))[char_], str3));

        BOOST_TEST(test("aaa", repeat(val(3), val(inf))[char_], str3));
        BOOST_TEST(test("aaaaaaaa", repeat(val(3), val(inf))[char_], str8));
        BOOST_TEST(!test("aaa", repeat(val(4), val(inf))[char_], str3));
    }

    {
        std::string str("aBcdeFGH");
        BOOST_TEST(test("abcdefgh", lower[repeat(8)[char_]], str));
        BOOST_TEST(test_delimited("A B C D E F G H ", upper[repeat(8)[char_]], str, space));
    }

    { // actions
        namespace phx = boost::phoenix;

        std::vector<char> v;
        v.push_back('a');
        v.push_back('a');
        v.push_back('a');
        v.push_back('a');
        BOOST_TEST(test("aaaa", repeat(4)[char_][_1 = phx::ref(v)]));
    }

    { // more actions
        namespace phx = boost::phoenix;

        std::vector<int> v;
        v.push_back(123);
        v.push_back(456);
        v.push_back(789);
        BOOST_TEST(test_delimited("123 456 789 ", repeat(3)[int_][_1 = phx::ref(v)], space));
    }

    // failing sub-generators
    {
        using namespace boost::assign;
        namespace karma = boost::spirit::karma;

        typedef std::pair<char, char> data;
        std::vector<data> v2, v3;
        v2 += std::make_pair('a', 'a'),
              std::make_pair('b', 'b'),
              std::make_pair('c', 'c'),
              std::make_pair('d', 'd'),
              std::make_pair('e', 'e'),
              std::make_pair('f', 'f'),
              std::make_pair('g', 'g');
        v3 += std::make_pair('a', 'a'),
              std::make_pair('b', 'b'),
              std::make_pair('c', 'c'),
              std::make_pair('d', 'd');

        karma::rule<spirit_test::output_iterator<char>::type, data()> r;

        r = &char_('d') << char_;
        BOOST_TEST(test("d", repeat[r], v2));

        r = !char_('d') << char_;
        BOOST_TEST(test("abcefg", repeat(6)[r], v2));
        BOOST_TEST(!test("", repeat(5)[r], v2));

        r = !char_('c') << char_;
        BOOST_TEST(test("abd", repeat(3)[r], v2));

        r = !char_('a') << char_;
        BOOST_TEST(test("bcdef", repeat(3, 5)[r], v2));
        BOOST_TEST(test("bcd", repeat(3, 5)[r], v3));
        BOOST_TEST(!test("", repeat(4, 5)[r], v3));

        BOOST_TEST(test("bcd", repeat(3, inf)[r], v3));
        BOOST_TEST(test("bcdefg", repeat(3, inf)[r], v2));
        BOOST_TEST(!test("", repeat(4, inf)[r], v3));
    }

    {
        // make sure user defined end condition is applied if no attribute
        // is passed in
        using namespace boost::assign;

        std::vector<char> v;
        v += 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h';
        BOOST_TEST(test("[6162636465666768]", 
            '[' << repeat[hex[action(v)]] << ']'));
    }

// we support Phoenix attributes only starting with V2.2
#if SPIRIT_VERSION >= 0x2020
    {
        namespace ascii = boost::spirit::ascii;
        namespace phoenix = boost::phoenix;

        char c = 'a';
        BOOST_TEST(test("bcd", repeat(3)[ascii::char_[_1 = ++phoenix::ref(c)]]));

        c = 'a';
        BOOST_TEST(test("bcd", repeat(3)[ascii::char_], ++phoenix::ref(c)));
    }
#endif

    return boost::report_errors();
}

