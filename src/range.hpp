/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/range.hpp

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
    `range_t` template defined here.
**/

#ifndef _TAPPER_RANGE_HPP_
#define _TAPPER_RANGE_HPP_

#include "base.hpp"

#include <limits>           // std::numeric_limits

namespace tapper {
namespace t {

/**
    Range is a combination of two bounds: lower and upper. Range can check if a number is in the
    range and extends its bounds to include the given value or another range. Also range can be
    stringified.
**/
template<
    typename              _value_t,
    _value_t              _dflt_min = std::numeric_limits< _value_t >::min(),
    _value_t              _dflt_max = std::numeric_limits< _value_t >::max()
>
struct range_t {

    STATIC_ASSERT( _dflt_min <= _dflt_max );

    using value_t  = _value_t;
    using myself_t = range_t< _value_t, _dflt_min, _dflt_max >;
    using error_t  = val_error_t;

    /** Constructs a new range with given or default boundaries. **/
    explicit
    range_t(
        value_t _min = _dflt_min,
        value_t _max = _dflt_max
    ): min( _min ), max( _max ) {
        assert( is_valid() );
    };

    /**
        Returns `true` if the range is valid (lower bound not greater than upper bound), and
        `false` otherwise.
    **/
    bool is_valid() const {
        return min <= max;
    };

    /**
        Returns `true` if the range includes the given value.
    **/
    bool includes( value_t value ) const {
        assert( is_valid() );
        return ( min <= value ) and ( value <= max );
    };

    /**
        Throws `val_error_t` exception if the given value is out of the range.
    **/
    void check( value_t value ) const {
        if ( not includes( value ) ) {
            ERR( "Out of range " << * this << "." );
        };
    };

    /**
        Extends the range boundaries to include the given value.
    **/
    myself_t & operator +=( value_t val ) {
        min = std::min( min, val );
        max = std::max( max, val );
        return * this;
    };

    /**
        Extends the range boundaries to include the given range.
    **/
    myself_t & operator +=( myself_t const & that ) {
        min = std::min( min, that.min );
        max = std::max( max, that.max );
        return * this;
    };

    string_t str() const {
        return STR( "[" << min << "…" << max << "]" );
    };

    value_t     min;        ///< The range's lower boundary.
    value_t     max;        ///< The range's upper boundary.

}; // struct range_t

}; // namespace t

}; // namespace tapper

#endif // _TAPPER_RANGE_HPP_

// end of file //
