/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/linux.cpp

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
    Linux wrappers implementation.
**/

#include "linux.hpp"

#include <map>
#include <stdexcept>

/*
    If the header is not available, the module will not provide any key names, but it will report
    almost valid `key_range`, so user can work using key codes.
*/
#if HAVE_LINUX_INPUT_EVENT_CODES_H
    #include "linux/input-event-codes.h"
#endif

#include "string.hpp"
#include "test.hpp"


namespace tapper {
namespace linux {

#if HAVE_LINUX_INPUT_EVENT_CODES_H

    /*
        If <linux/input-event-codes.h> is available, make sure my constants match to Linux ones.
    */

    STATIC_ASSERT( ev_syn == EV_SYN );
    STATIC_ASSERT( ev_key == EV_KEY );

    STATIC_ASSERT( syn_report == SYN_REPORT );

    STATIC_ASSERT( key_t::none         == KEY_RESERVED );
    STATIC_ASSERT( key_t::max          == KEY_MAX      );
    STATIC_ASSERT( key_t::btn_left     == BTN_LEFT     );
    STATIC_ASSERT( key_t::btn_right    == BTN_RIGHT    );
    STATIC_ASSERT( key_t::btn_middle   == BTN_MIDDLE   );

#endif // HAVE_LINUX_INPUT_EVENT_CODES_H

STATIC_ASSERT( key_t::min     <= key_t::btn_min                                            );
STATIC_ASSERT( key_t::btn_min <= key_t::btn_left   and key_t::btn_left   <= key_t::btn_max );
STATIC_ASSERT( key_t::btn_min <= key_t::btn_right  and key_t::btn_right  <= key_t::btn_max );
STATIC_ASSERT( key_t::btn_min <= key_t::btn_middle and key_t::btn_middle <= key_t::btn_max );
STATIC_ASSERT( key_t::btn_max <= key_t::max                                                );

/// “Key name → key” mapping.
using name2key_t  = std::map< string_t, key_t >;

/**
    “Key → key names” mapping. Note the plural form: “key names”. Some Linux event codes have
    aliases, e. g. `KEY_COFFEE` and `KEY_SCREENLOCK` are two names for the same event. Such mapping
    allows to have more than one name for a key.
**/
class key2names_t: public std::map< key_t, strings_t > {
    public:
        key2names_t( std::initializer_list< std::pair< key_t, string_t > > const & list ) {
            for ( auto const & it: list ) {
                auto ins = insert( { it.first, { it.second } } );
                if ( not ins.second ) {
                    ins.first->second.push_back( it.second );
                };
            };
        };
};

/// Returns “Key name → key” mapping.
static
name2key_t const &
_name2key(
) {
    static name2key_t const name2key {
        #if HAVE_LINUX_INPUT_EVENT_CODES_H
            #define X( NAME ) { #NAME, key_t( NAME ) },
            #include "input-event-names.def"
            #undef X
        #endif // HAVE_LINUX_INPUT_EVENT_CODES_H
    };
    return name2key;
}; // _name2key

#if HAVE_LINUX_INPUT_EVENT_CODES_H
    TEST(
        auto const & map = _name2key();
        // Make sure the map contains Taper keys, not Linux event codes:
        ASSERT_EQ( map.at( "KEY_ESC" ), key_t( 1 ) );   // key_min, not KEY_ESC
        // Make sure the map includes names and aliases:
        ASSERT_EQ( map.at( "KEY_COFFEE" ), map.at( "KEY_SCREENLOCK" ) );
    );
#endif // HAVE_LINUX_INPUT_EVENT_CODES_H

/// Returns “Key → key names” mapping.
static
key2names_t const &
_key2names(
) {
    static key2names_t const key2names {
        #if HAVE_LINUX_INPUT_EVENT_CODES_H
            #define X( NAME ) { key_t( NAME ), #NAME },
            #include "input-event-names.def"
            #undef X
        #endif // HAVE_LINUX_INPUT_EVENT_CODES_H
    };
    return key2names;
}; // _key2names

#if HAVE_LINUX_INPUT_EVENT_CODES_H
    TEST(
        auto const & map = _key2names();
        // Make sure the map includes aliases:
        auto const names = map.at( key_t( KEY_COFFEE ) );
        ASSERT_EQ( names.at( 0 ), "KEY_COFFEE" );
        ASSERT_EQ( names.at( 1 ), "KEY_SCREENLOCK" );
    );
#endif // HAVE_LINUX_INPUT_EVENT_CODES_H

key_t::range_t
key_range(
) {
    return key_t::range_t();
}; // key_range

TEST(
    auto r = key_range();
    ASSERT_EQ( r.min, key_t::min );
    ASSERT_EQ( r.max, key_t::max );
);

keys_t
keys(
) {
    keys_t keys;
    for ( auto it: _key2names() ) {
        keys.insert( it.first );
    };
    return keys;
};


#if HAVE_LINUX_INPUT_EVENT_CODES_H
    TEST(
        auto set = keys();
        ASSERT_EQ( set.count( key_t( KEY_1 ) ), 1U );
        ASSERT_EQ( set.count( key_t( KEY_2 ) ), 1U );
    );
#endif // HAVE_LINUX_INPUT_EVENT_CODES_H

key_t
key(
    string_t const & name
) {
    auto const & map = _name2key();
    for ( auto const & n: { uc( name ), uc( "KEY_" + name ), uc( "BTN_" + name ) } ) {
        auto it = map.find( n );
        if ( it != map.end() ) {
            return it->second;
        };
    };
    return key_t();
};

#if HAVE_LINUX_INPUT_EVENT_CODES_H
    TEST(
        ASSERT_EQ( key( "KEY_ESC" ), key_t( 1 ) );
        ASSERT_EQ( key( "key_esc" ), key_t( 1 ) );  // Lowercase works.
        ASSERT_EQ( key( "ESC" ),     key_t( 1 ) );  // Short form works.
        ASSERT_EQ( key( "Esc" ),     key_t( 1 ) );  // Mixed case works.
    );
#endif // HAVE_LINUX_INPUT_EVENT_CODES_H

string_t
key_name(
    key_t key
) {
    auto names = key_names( key );
    return names.empty() ? "" : names.front();
};

#if HAVE_LINUX_INPUT_EVENT_CODES_H
    TEST(
        ASSERT_EQ( key_name( key_t( 1 ) ), "KEY_ESC" );
    );
#endif // HAVE_LINUX_INPUT_EVENT_CODES_H

strings_t
key_names(
    key_t key
) {
    key_range().check( key );
    auto const & map = _key2names();
    auto it = map.find( key );
    if ( it == map.end() ) {
        return {};
    };
    return it->second;
};

}; // namespace linux
}; // namespace tapper

// end of file //
