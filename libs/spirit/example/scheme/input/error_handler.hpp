/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SEXPR_ERROR_HANDLER)
#define BOOST_SPIRIT_SEXPR_ERROR_HANDLER

#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <support/line_pos_iterator.hpp>
#include <string>
#include <iostream>

namespace scheme { namespace input
{
    template <typename Iterator>
    struct error_handler
    {
        template <typename, typename, typename, typename>
        struct result { typedef void type; };

        std::string source_file;
        error_handler(std::string const& source_file = "")
          : source_file(source_file) {}

        void operator()(
            Iterator first, Iterator last,
            Iterator err_pos, boost::spirit::info const& what) const
        {
            Iterator eol = err_pos;
            int line = get_line(err_pos);

            if (source_file != "")
                std::cerr << source_file;

            if (line != -1)
            {
                if (source_file != "")
                    std::cerr << '(' << line << ')';
                else
                    std::cerr << '(' << line << ')';
            }

            std::cerr << " : Error! Expecting "  << what;
            //~ if (line != -1)
            //~ {
                //~ std::size_t col = get_column(first, err_pos, 4);
                //~ std::cerr << " near column " << col << std::endl;
            //~ }
            //~ else
            //~ {
                std::cerr << std::endl;
            //~ }
        }
    };
}}

#endif
