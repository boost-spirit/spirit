//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_QI_UNUSED_SKIPPER_JUL_25_2009_0921AM)
#define BOOST_SPIRIT_QI_UNUSED_SKIPPER_JUL_25_2009_0921AM

#include <boost/spirit/home/support/unused.hpp>

namespace boost { namespace spirit { namespace qi { namespace detail
{
    template <typename Skipper>
    struct unused_skipper : unused_type
    {
        unused_skipper(Skipper const& skipper)
          : skipper(skipper) {}
        Skipper const& skipper;
    };

    // If a surrounding lexeme[] directive was specified, the current
    // skipper is of the type unused_skipper. In this case we 
    // re-activate the skipper which was active before the skip[]
    // directive.
    template <typename Skipper, typename Default>
    inline Skipper const& 
    get_skipper(unused_skipper<Skipper> const& u, Default const&)
    {
        return u.skipper;
    }

    // If no surrounding lexeme[] directive was specified we activate
    // a default skipper to use.
    template <typename Skipper, typename Default>
    inline Default const& 
    get_skipper(Skipper const&, Default const& d)
    {
        return d;
    }

}}}}

#endif
