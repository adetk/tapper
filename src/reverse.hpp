/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/reverse.hpp

    Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.

    This file is part of Tapper.

    Tapper is free software: you can redistribute it and/or modify it under the terms of the GNU
    General Public License as published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    Tapper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License along with Tapper.  If not,
    see <https://www.gnu.org/licenses/>.

    SPDX-License-Identifier: GPL-3.0-or-later

    ---------------------------------------------------------------------- copyright and license ---
*/

/** @file
    `reverse_map_t` template class and `reverse` template function.
**/

#ifndef _TAPPER_REVERSE_HPP_
#define _TAPPER_REVERSE_HPP_

#include "config.h"

#include <map>

namespace tapper {

/** Construct reverse map type. **/
template< typename map_t >
using reverse_map_t = std::map< typename map_t::mapped_type, typename map_t::key_type >;

/** Reverse a map. **/
template< typename map_t >
static
reverse_map_t< map_t >
reverse(
    map_t const & map
) {
    reverse_map_t< map_t > reversed_map;
    for ( auto & it: map ) {
        reversed_map.insert( { it.second, it.first } );
    };
    return reversed_map;
};

}; // namespace tapper

#endif // _TAPPER_REVERSE_HPP_

// end of file //
