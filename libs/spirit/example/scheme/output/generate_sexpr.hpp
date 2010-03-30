//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SCHEME_OUTPUT_GENERATE_SEXPR_MAR_29_2010_1210PM)
#define SCHEME_OUTPUT_GENERATE_SEXPR_MAR_29_2010_1210PM

#include "../utree.hpp"
#include "../output/sexpr.hpp"

namespace scheme { namespace output
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputStream>
    bool generate_sexpr(OutputStream& os, utree const& tree);

    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputStream>
    bool generate_sexpr_list(OutputStream& os, utree const& tree);
}}

#endif


