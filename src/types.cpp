/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/types.cpp

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
    Tapper-specific types implementation.
**/

#include "types.hpp"

#include <algorithm>    // std::min, std::max, std::copy
#include <iterator>     // std::back_inserter
#include <regex>
#include <stdexcept>

#include "string.hpp"
#include "test.hpp"

namespace tapper {

// -------------------------------------------------------------------------------------------------
// layout_t
// -------------------------------------------------------------------------------------------------

template<>
layout_t
val< layout_t >(
    string_t const & string
) {
    using error_t = val_error_t;
    try {
        auto result = layout_t( val< layout_t::rep_t >( string ) );
        layout_range_t().check( result );
        return result;
    } catch ( error_t const & ex ) {
        ERR( "Bad layout index " << q( string ) << ": " << ex.what() );
    };
};

TEST(
    ASSERT_EQ( val< layout_t >( "1" ).index, 1U );
);

// -------------------------------------------------------------------------------------------------
// key_t
// -------------------------------------------------------------------------------------------------

template<>
key_t
val< key_t >(
    string_t const & string
) {
    TRACE();
    using error_t = val_error_t;
    try {
        auto result = key_t( val< key_t::rep_t >( string ) );
        DBG( result << " " << key_t::range_t() );
        key_t::range_t().check( result );
        return result;
    } catch ( error_t const & ex ) {
        ERR( "Bad key code " << q( string ) << ": " << ex.what() );
    };
};

TEST(
    ASSERT_EQ( val< key_t >( "1" ), key_t( 1 ) );
);

// -------------------------------------------------------------------------------------------------
// action_t
// -------------------------------------------------------------------------------------------------

string_t
action_t::str(
) const {
    using tapper::str;
    string_t result = "(*noop*)";
    switch ( _type ) {
        case type_t::none: {
        } break;
        case type_t::activate_layout: {
            // TODO: What if index is 0?
            result = "@" + str( _layout );
        } break;
        case type_t::emit_key_tap: {
            // TODO: What if code is 0?
            result = str( _key );
        } break;
    };
    return result;
};

TEST(
    ASSERT_EQ( str( action_t() ), "(*noop*)" );
    ASSERT_EQ( str( action_t::activate_layout( layout_t( 1 ) ) ), "@1" );
    ASSERT_EQ( str( action_t::activate_layout( layout_t( 2 ) ) ), "@2" );
    ASSERT_EQ( str( action_t::emit_key_tap( key_t( 1 ) ) ), "1" );
    ASSERT_EQ( str( action_t::emit_key_tap( key_t( 2 ) ) ), "2" );
);

template<>
action_t
val< action_t >(
    string_t const & string
) {
    action_t result;
    if ( false ) {
    } else if ( front( string ) == '@' ) {
        auto layout = val< layout_t >( string.substr( 1 ) );
        result = action_t::activate_layout( layout );
    } else {
        auto key = val< key_t >( string );
        result = action_t::emit_key_tap( key );
    };
    return result;
};

template<>
string_t
str(
    actions_t const & actions
) {
    strings_t strings;
    std::transform(
        actions.begin(), actions.end(),
        std::back_inserter( strings ),
        [] ( action_t const & action ) { return str( action ); }
    );
    return join( ", ", strings );
};

TEST(

    actions_t empty;
    ASSERT_EQ( str( empty ), "" );

    actions_t one { action_t::activate_layout( layout_t( 1 ) ) };
    ASSERT_EQ( str( one ), "@1" );

    actions_t two {
        action_t::emit_key_tap( key_t( 1 ) ),
        action_t::activate_layout( layout_t( 1 ) )
    };
    ASSERT_EQ( str( two ), "1, @1" );

);

template<>
actions_t
val< actions_t >(
    string_t const & string
) {
    actions_t result;
    using error_t = val_error_t;
    try {
        for ( auto action: split( ',', string ) ) {
            action = trim( action );
            if ( not action.empty() ) {
                result.push_back( val< action_t >( action ) );
            };
        };
    } catch ( error_t const & ex ) {
        ERR( "Bad actions " << q( string ) << ": " << ex.what() );
    };
    return result;
};

/**
    Returns `true`, if there are any layout activations in given actions, and `false` otherwise.
**/
bool
has_any_layout_activations(
    actions_t const & actions
) {
    auto it = std::find_if(
        actions.begin(),
        actions.end(),
        [] ( action_t const & action ) {
            return action.type() == action_t::type_t::activate_layout;
        }
    );
    return it != actions.end();
};

/**
    Returns `true`, if there are any key emit in given actions, and `false` otherwise.
**/
bool
has_any_key_emits(
    actions_t const & actions
) {
    auto it = std::find_if(
        actions.begin(),
        actions.end(),
        [] ( action_t const & action ) {
            return action.type() == action_t::type_t::emit_key_tap;
        }
    );
    return it != actions.end();
};

// -------------------------------------------------------------------------------------------------
// assignments_t
// -------------------------------------------------------------------------------------------------

template<>
string_t
str(
    assignments_t const & value
) {
    string_t result;
    string_t comma = "";
    for ( auto & assignment: value ) {
        result += comma + str( assignment.first ) + ": " + str( assignment.second );
        comma = ", ";
    };
    return result.empty() ? "{}" : "{ " + result + " }";
};

/**
    Returns `true`, if there are any key emit actions in given key assignments, and `false`
    otherwise.
**/
bool
has_any_layout_activations(
    assignments_t const & assignments
) {
    auto it = std::find_if(
        assignments.begin(),
        assignments.end(),
        [] ( assignments_t::value_type const & assignment ) {
            return has_any_layout_activations( assignment.second );
        }
    );
    return it != assignments.end();
};

/**
    Returns `true`, if there are any key emit actions in given key assignments, and `false`
    otherwise.
**/
bool
has_any_key_emits(
    assignments_t const & assignments
) {
    auto it = std::find_if(
        assignments.begin(),
        assignments.end(),
        [] ( assignments_t::value_type const & assignment ) {
            return has_any_key_emits( assignment.second );
        }
    );
    return it != assignments.end();
};

/**
    Merge two assignments. The key assignments in the second argument overrides key assignments in
    the first argument. Empty vector is treated specially: it means "unassign the key".
**/
assignments_t &
operator +=(
    assignments_t &       lhs,
    assignments_t const & rhs
) {
    for ( auto & assignment: rhs ) {
        if ( assignment.second.empty() ) {
            lhs.erase( assignment.first );
        } else {
            lhs.insert( assignment );
        };
    };
    return lhs;
};

assignments_t
operator +(
    assignments_t const & lhs,
    assignments_t const & rhs
) {
    auto r = lhs;
    return r += rhs;
};

}; // namespace tapper

// end of file //
