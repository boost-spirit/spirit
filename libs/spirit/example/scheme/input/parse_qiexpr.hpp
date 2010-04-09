//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_PARSE_QIEXPR)
#define BOOST_SPIRIT_PARSE_QIEXPR

#include "../utree.hpp"

namespace scheme { namespace input
{
    template <typename String>
    bool parse_qiexpr(String const& str, utree& result);
}}

#endif


