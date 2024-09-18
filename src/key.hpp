/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/key.hpp

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
    `key_t` template defined here.
**/

#ifndef _TAPPER_KEY_HPP_
#define _TAPPER_KEY_HPP_

#include "base.hpp"

#include <limits>

#include "range.hpp"

namespace tapper {
namespace t {

/**
    Key template. Tapper operates with Linux key codes, X Window System key codes, X Window System
    button codes, they all have much in common. These template provides this common stuff and
    avoids code duplication.

    Key is a class to avoid mixing various keys, e. g. Linux key can't be passed to a function
    expecting X key.
**/
template<
    typename _rep_t,
    _rep_t   _min = 0,                                      ///< Min valid key code.
    _rep_t   _max = std::numeric_limits< _rep_t >::max()    ///< Max valid key code.
>
class key_t {

    STATIC_ASSERT( _min >= 0 );
    STATIC_ASSERT( _max >= _min );

    public:

        using rep_t    = _rep_t;
        using myself_t = key_t< _rep_t, _min, _max >;

        static rep_t constexpr none = 0;        ///< Special key code that means "no key".
        static rep_t constexpr min  = _min;     ///< Minimal key code.
        static rep_t constexpr max  = _max;     ///< Maximal key code.

        /** Key range. **/
        struct range_t: t::range_t< rep_t, min, max > {

            using parent_t = t::range_t< rep_t, min, max >;
            using myself_t = range_t;

            using parent_t::parent_t;

            /** Throws `val_error_t` exception if the given key is out of range `[min…max]`. **/
            void check( key_t key ) { parent_t::check( key.code() ); };

        };

        using range_p = ptr_t< range_t >;

        /** Constructs key from key code. By default "no key" is constructed. **/
        explicit key_t( rep_t code = none ): _code( code ) {};

        /** Returns key code. **/
        rep_t code() const { return _code; };

        /** Comparison operator, so keys can be compared. **/
        bool operator ==( myself_t that ) const { return _code == that._code; };

        /** Less operator, required by `std::set`. **/
        bool operator <( myself_t that ) const { return _code < that._code; };

        /** Stringifies key (by stringifying the key code). **/
        string_t str() const { return tapper::str( _code ); };

    protected:

        bool is_valid() const {
            return range_t().includes( _code );
        };

        rep_t _code;        ///< The key code.

}; // class key_t

}; // namespace t
}; // namespace tapper

#endif // _TAPPER_KEY_HPP_

// end of file //
