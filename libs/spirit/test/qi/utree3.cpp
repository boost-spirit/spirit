// Copyright (c) 2001-2010 Hartmut Kaiser
// Copyright (c) 2001-2010 Joel de Guzman
// Copyright (c)      2010 Bryce Lelbach
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_utree.hpp>
#include <boost/mpl/print.hpp>

#include <sstream>

#include "test.hpp"

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

int main()
{
    using spirit_test::test_attr;
    using boost::spirit::utree;
    using boost::spirit::utree_type;
    using boost::spirit::utf8_string_range_type;
    using boost::spirit::utf8_symbol_type;
    using boost::spirit::utf8_string_type;

    using boost::spirit::qi::real_parser;
    using boost::spirit::qi::strict_real_policies;
    using boost::spirit::qi::digit;
    using boost::spirit::qi::char_;
    using boost::spirit::qi::string;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::double_;
    using boost::spirit::qi::space;
    using boost::spirit::qi::space_type;
    using boost::spirit::qi::rule;
    using boost::spirit::qi::as;
    using boost::spirit::qi::lexeme;

    // alternatives
    {
        typedef real_parser<double, strict_real_policies<double> >
            strict_double_type;
        strict_double_type const strict_double = strict_double_type();

        utree ut;
        BOOST_TEST(test_attr("10", strict_double | int_, ut) &&
            ut.which() == utree_type::int_type && check(ut, "10"));
        ut.clear();
        BOOST_TEST(test_attr("10.2", strict_double | int_, ut) &&
            ut.which() == utree_type::double_type && check(ut, "10.2"));

        rule<char const*, boost::variant<int, double>()> r1 = strict_double | int_;
        ut.clear();
        BOOST_TEST(test_attr("10", r1, ut) &&
            ut.which() == utree_type::int_type && check(ut, "10"));
        ut.clear();
        BOOST_TEST(test_attr("10.2", r1, ut) &&
            ut.which() == utree_type::double_type && check(ut, "10.2"));

        rule<char const*, utree()> r2 = strict_double | int_;
        ut.clear();
        BOOST_TEST(test_attr("10", r2, ut) &&
            ut.which() == utree_type::int_type && check(ut, "10"));
        ut.clear();
        BOOST_TEST(test_attr("10.2", r2, ut) &&
            ut.which() == utree_type::double_type && check(ut, "10.2"));
        
        rule<char const*, utree::list_type()> r3 = strict_double | int_;
        ut.clear();
        BOOST_TEST(test_attr("10", r3, ut) &&
            ut.which() == utree_type::list_type && check(ut, "( 10 )"));
        ut.clear();
        BOOST_TEST(test_attr("10.2", r3, ut) &&
            ut.which() == utree_type::list_type && check(ut, "( 10.2 )"));
    }

    // optionals
    {
        utree ut;
        BOOST_TEST(test_attr("x", -char_, ut) &&
            ut.which() == utree_type::string_type && check(ut, "\"x\""));
        ut.clear();
        BOOST_TEST(test_attr("", -char_, ut) &&
            ut.which() == utree_type::invalid_type && 
            check(ut, "<invalid>"));
    }

    // as_string
    {
        using boost::spirit::qi::as_string;

        utree ut;
        BOOST_TEST(test_attr("xy", as_string[char_ >> char_], ut) &&
            ut.which() == utree_type::string_type && check(ut, "\"xy\""));
        ut.clear();

        BOOST_TEST(test_attr("ab1.2", as_string[*~digit] >> double_, ut) &&
            ut.which() == utree_type::list_type && check(ut, "( \"ab\" 1.2 )"));
        ut.clear();

        BOOST_TEST(test_attr("xy", as_string[*char_], ut) &&
            ut.which() == utree_type::string_type && check(ut, "\"xy\""));
        ut.clear();

        BOOST_TEST(test_attr("x,y", as_string[char_ >> ',' >> char_], ut) &&
            ut.which() == utree_type::string_type && check(ut, "\"xy\""));
        ut.clear();

        BOOST_TEST(test_attr("x,y", char_ >> ',' >> char_, ut) &&
            ut.which() == utree_type::list_type && check(ut, "( \"x\" \"y\" )"));
        ut.clear();

        BOOST_TEST(test_attr("a,b1.2", as_string[~digit % ','] >> double_, ut) &&
            ut.which() == utree_type::list_type && check(ut, "( \"ab\" 1.2 )"));
        ut.clear();

        BOOST_TEST(test_attr("a,b1.2", ~digit % ',' >> double_, ut) &&
            ut.which() == utree_type::list_type && check(ut, "( \"a\" \"b\" 1.2 )"));
        ut.clear();
    }

    return boost::report_errors();
}
