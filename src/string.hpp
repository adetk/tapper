/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/string.hpp

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

    String manipulating utilities interface. Type `string_t` is defined in `"base.hpp"` header, so
    other header files should *not* include this one, only `*.cpp` files should include it.

    @sa string.cpp
**/

#ifndef _TAPPER_STRING_HPP_
#define _TAPPER_STRING_HPP_

#include "base.hpp"

#include <limits>
#include <regex>

namespace tapper {

string_t    chop( string_t const & str, string_t const & sfx );
strings_t   concat( string_t const & lhs, strings_t const & rhs );
strings_t   concat( strings_t const & lhs, strings_t const & rhs );
string_t    errstr( int err, string_t const & msg );
size_t      find( string_t const & needle, strings_t const & haystack );
char_t      front( string_t const & string );
bool        has_prefix( string_t const & string, string_t const & prefix );
bool        is_one_of( char_t ch, string_t const & string );
string_t    join( string_t const & joiner, strings_t const & strings );
string_t    lc( string_t const & str );
string_t    left( string_t const & str, size_t len );
bool        like_cardinal( string_t const & str, size_t pos = 0 );
bool        like_integer( string_t const & str );
std::cmatch match( string_t const &  string, string_t const &  rex );
string_t    pad_l( string_t const & str, size_t len, char pad = ' ' );
string_t    pad_r( string_t const & str, size_t len, char pad = ' ' );
string_t    q( string_t const & str );
string_t    right( string_t const & str, size_t len );
strings_t   split( char chr, string_t const & str, size_t limit = 0 );
string_t    trim( string_t const & str );
string_t    trim_l( string_t const & str );
string_t    trim_r( string_t const & str );
string_t    uc( string_t const & str );

};  // namespace tapper

#endif // _TAPPER_STRING_HPP_

// end of file //
