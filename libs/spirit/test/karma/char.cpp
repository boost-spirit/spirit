//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//#define KARMA_FAIL_COMPILATION

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_action.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>

#include "test.hpp"

using namespace spirit_test;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    using namespace boost::spirit;
    using namespace boost::phoenix;

    {
        using namespace boost::spirit::ascii;

        BOOST_TEST(test("x", 'x'));
        BOOST_TEST(test(L"x", L'x'));
        BOOST_TEST(!test("x", 'y'));
        BOOST_TEST(!test(L"x", L'y'));

        BOOST_TEST(test("x", "x"));
        BOOST_TEST(test(L"x", L"x"));
        BOOST_TEST(!test("x", "y"));
        BOOST_TEST(!test(L"x", L"y"));

        BOOST_TEST(test("x", char_, 'x'));
        BOOST_TEST(test(L"x", char_, L'x'));
        BOOST_TEST(!test("x", char_, 'y'));
        BOOST_TEST(!test(L"x", char_, L'y'));

        BOOST_TEST(test("x", char_('x')));
        BOOST_TEST(!test("x", char_('y')));

        BOOST_TEST(test("x", char_('x'), 'x'));
        BOOST_TEST(!test("", char_('y'), 'x'));

        BOOST_TEST(test("x", char_("x")));

        BOOST_TEST(test("a", char_('a', 'z'), 'a'));
        BOOST_TEST(test("b", char_('a', 'z'), 'b'));
        BOOST_TEST(!test("", char_('a', 'z'), 'A'));

        BOOST_TEST(test("a", char_("a-z"), 'a'));
        BOOST_TEST(test("b", char_("a-z"), 'b'));
        BOOST_TEST(!test("", char_("a-z"), 'A'));

#if defined(KARMA_FAIL_COMPILATION)
        BOOST_TEST(test("x", char_));           // anychar without a parameter doesn't make any sense
        BOOST_TEST(test("", char_('a', 'z')));  // char sets without attribute neither
#endif
    }

    {
        using namespace boost::spirit::standard_wide;

        BOOST_TEST(test(L"x", 'x'));
        BOOST_TEST(test(L"x", L'x'));
        BOOST_TEST(!test(L"x", 'y'));
        BOOST_TEST(!test(L"x", L'y'));

        BOOST_TEST(test(L"x", "x"));
        BOOST_TEST(test(L"x", L"x"));
        BOOST_TEST(!test(L"x", "y"));
        BOOST_TEST(!test(L"x", L"y"));

        BOOST_TEST(test(L"x", char_, 'x'));
        BOOST_TEST(test(L"x", char_, L'x'));
        BOOST_TEST(!test(L"x", char_, 'y'));
        BOOST_TEST(!test(L"x", char_, L'y'));

        BOOST_TEST(test(L"x", char_('x')));
        BOOST_TEST(test(L"x", char_(L'x')));
        BOOST_TEST(!test(L"x", char_('y')));
        BOOST_TEST(!test(L"x", char_(L'y')));

        BOOST_TEST(test(L"x", char_(L'x'), L'x'));
        BOOST_TEST(!test(L"", char_('y'), L'x'));

        BOOST_TEST(test(L"x", char_(L"x")));

#if defined(KARMA_FAIL_COMPILATION)
        BOOST_TEST(test("x", char_));           // anychar without a parameter doesn't make any sense
#endif
    }

    {
        using namespace boost::spirit::ascii;

        BOOST_TEST(test(" ", space));
        BOOST_TEST(test(L" ", space));
        BOOST_TEST(!test("\t", space));
        BOOST_TEST(!test(L"\t", space));

        BOOST_TEST(test(" ", space, ' '));
        BOOST_TEST(test(L" ", space, L' '));
        BOOST_TEST(test("\t", space, '\t'));
        BOOST_TEST(test(L"\t", space, L'\t'));

        BOOST_TEST(!test("", space, 'x'));
        BOOST_TEST(!test(L"", space, L'x'));
    }

    {
        using namespace boost::spirit::standard_wide;

        BOOST_TEST(test(" ", space));
        BOOST_TEST(test(L" ", space));
        BOOST_TEST(!test("\t", space));
        BOOST_TEST(!test(L"\t", space));

        BOOST_TEST(test(" ", space, ' '));
        BOOST_TEST(test(L" ", space, L' '));
        BOOST_TEST(test("\t", space, '\t'));
        BOOST_TEST(test(L"\t", space, L'\t'));

        BOOST_TEST(!test("", space, 'x'));
        BOOST_TEST(!test(L"", space, L'x'));
    }

    {
        using namespace boost::spirit::ascii;

        BOOST_TEST(test_delimited("x ", 'x', ' '));
        BOOST_TEST(test_delimited(L"x ", L'x', L' '));
        BOOST_TEST(!test_delimited("x ", 'y', ' '));
        BOOST_TEST(!test_delimited(L"x ", L'y', L' '));

        BOOST_TEST(test_delimited("x ", 'x', ' '));
        BOOST_TEST(test_delimited(L"x ", L'x', L' '));
        BOOST_TEST(!test_delimited("x ", 'y', ' '));
        BOOST_TEST(!test_delimited(L"x ", L'y', L' '));

        BOOST_TEST(test_delimited("x ", char_, 'x', ' '));
        BOOST_TEST(test_delimited(L"x ", char_, L'x', L' '));
        BOOST_TEST(!test_delimited("x ", char_, 'y', ' '));
        BOOST_TEST(!test_delimited(L"x ", char_, L'y', L' '));

        BOOST_TEST(test_delimited("x ", char_('x'), ' '));
        BOOST_TEST(!test_delimited("x ", char_('y'), ' '));

        BOOST_TEST(test_delimited("x ", char_('x'), 'x', ' '));
        BOOST_TEST(!test_delimited("", char_('y'), 'x', ' '));

        BOOST_TEST(test_delimited("x ", char_("x"), ' '));

#if defined(KARMA_FAIL_COMPILATION)
        BOOST_TEST(test_delimited("x ", char_, ' '));   // anychar without a parameter doesn't make any sense
#endif
    }

    {   // pre-delimiting
        {
            std::string generated;
            std::back_insert_iterator<std::string> it(generated);
            BOOST_TEST(karma::generate_delimited(it, '_', '^'
              , karma::delimit_flag::predelimit));
            BOOST_TEST(generated == "^_^");
        }
        {
            using namespace boost::spirit::standard_wide;
            std::basic_string<wchar_t> generated;
            std::back_insert_iterator<std::basic_string<wchar_t> > it(generated);
            BOOST_TEST(karma::generate_delimited(it, char_, L'.'
              , karma::delimit_flag::predelimit, L'x'));
            BOOST_TEST(generated == L".x.");
        }
    }

    // action tests
    {
        using namespace boost::spirit::ascii;

        BOOST_TEST(test("x", char_[_1 = val('x')]));
        BOOST_TEST(!test("x", char_[_1 = val('y')]));
    }

    {   // lazy chars
        namespace ascii = boost::spirit::ascii;
        namespace wide = boost::spirit::standard_wide;

        using namespace boost::phoenix;

        BOOST_TEST((test("x", ascii::char_(val('x')))));
        BOOST_TEST((test(L"x", wide::char_(val(L'x')))));

        BOOST_TEST((test("x", ascii::char_(val('x')), 'x')));
        BOOST_TEST((test(L"x", wide::char_(val(L'x')), L'x')));

        BOOST_TEST((!test("", ascii::char_(val('y')), 'x')));
        BOOST_TEST((!test(L"", wide::char_(val(L'y')), L'x')));
    }

    return boost::report_errors();
}
