/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/types.hpp

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
    Tapper-specific types interface. See also `settings.hpp` for few more Tapper-specific types,
    which are close related to GSettings.

    @sa types.cpp
**/

#ifndef _TAPPER_TYPES_HPP_
#define _TAPPER_TYPES_HPP_

#include "base.hpp"

#include <limits>           // std::numeric_limits
#include <map>
#include <set>
#include <vector>

#include "linux.hpp"
#include "range.hpp"

namespace tapper {

/**
    @defgroup ttypes Tapper-specific types
**/

/// @{

/**
    Integer type to represent event's time in milliseconds.
**/
using time_t = uint_t;

/**
    Layout index.
**/
struct layout_t {
    using rep_t    = uint_t;
    using myself_t = layout_t;
    explicit layout_t( rep_t _index = 0 ): index( _index ) {};
    bool operator <( myself_t that ) const { return index < that.index; };
    string_t str() const { return tapper::str( index ); };
    rep_t index;
};

/** @ingroup val
    Converts string to `layout_t` value.
**/
template<> layout_t val< layout_t >( string_t const & string );

/** Set of layouts. **/
using layouts_t = std::set< layout_t >;
using layouts_p = ptr_t< layouts_t >;

/**
    Layout index range.
**/
struct layout_range_t: t::range_t< layout_t::rep_t, 1 > {
    using parent_t = myself_t;
    using myself_t = layout_range_t;
    using parent_t::parent_t;
    void check( layout_t layout ) { parent_t::check( layout.index ); };
}; // struct layout_range_t

/*
    Tapper uses Linux kernel key codes.
*/
using linux::key_t;
using linux::keys_t;
using linux::keys_p;

/** @ingroup val
    Converts string to `key_t` value.
**/
//~ template<> key_t val< key_t >( string_t const & string ); // TODO

/** Key state: pressed or released. **/
enum class key_state_t {
    released,
    pressed,
};

/**
    Action.
**/
class action_t {

    public:

        enum class type_t {
            none,
            activate_layout,
            emit_key_tap,
        };

        action_t(): _type( type_t::none ) {};

        /** Constructs new `activate_layot` action. **/
        static action_t activate_layout( layout_t layout ) {
            return action_t( type_t::activate_layout, layout, key_t() );
        };

        /** Constructs new `emit_key_tap` action. **/
        static action_t emit_key_tap( key_t key ) {
            return action_t( type_t::emit_key_tap, layout_t(), key );
        };

        /** Returns action type. */
        type_t type() const { return _type; };

        /**
            Returns layout of `activate_layout` action. Do not call this method if action type is
            not `activate_layout`.
        **/
        layout_t layout() const {
            assert( _type == type_t::activate_layout );
            return _layout;
        };

        /**
            Returns key of `emit_key_tap` action. Do not call this method if action type is not
            `emit_key_tap`.
        **/
        key_t key() const {
            assert( _type == type_t::emit_key_tap );
            return _key;
        };

        /**
            Converts `action_t` value to string. Stringification of `activate_layout` action is `@`
            character followed by stringified layout index; stringification of `emit_key_tap`
            action is `#` character followed by stringified key code.
        **/
        string_t str() const;

    private:

        /** Private constrictor. **/
        explicit action_t( type_t type, layout_t layout, key_t key ):
            _type( type ), _layout( layout ), _key( key ) {};

        type_t      _type;      ///< Action type.
        layout_t    _layout;
        key_t       _key;

};

/** @ingroup val
    Converts string to `action_t` value. It is reverse operation for `str( action_t )`, so

    @code
        val< action_t >( str( action ) == action
    @endcode

    for any valid `action` (action of type `none` is not considered valid).

    @note Neither layout names nor key names are recognized.
**/
template<> action_t val< action_t >( string_t const & string );

/** Sequence of actions. **/
using actions_t = std::vector< action_t >;

/** @ingroup str
    Converts `actions_t` value to string.
**/
template<> string_t str( actions_t const & value );

/** @ingroup val
    Converts string to `actions_t` value.
**/
template<> actions_t val< actions_t >( string_t const & string );

bool has_any_layout_activations( actions_t const & actions );
bool has_any_key_emits( actions_t const & actions );

/** Key assignments. **/
using assignments_t = std::map< key_t, actions_t >;

/** @ingroup str
    Converts `assignments_t` value to string.
**/
template<> string_t str( assignments_t const & assignments );
bool has_any_layout_activations( assignments_t const & assignments );
bool has_any_key_emits( assignments_t const & assignments );
assignments_t & operator +=( assignments_t & lhs, assignments_t const & rhs );

/** @} **/

}; // namespace tapper

#endif // _TAPPER_TYPES_HPP_

// end of file //
