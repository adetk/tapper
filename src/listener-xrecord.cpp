/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/listener-xrecord.cpp

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
    `listener::xrecord_t` class implementation.
**/

#include "listener-xrecord.hpp"
#include "listener-xrecord.h"

#include <limits>

#include <byteswap.h>       // bswap_64
#include <X11/Xproto.h>     // xEvent

#include "string.hpp"
#include "test.hpp"

tapper::listener_t *
listener_xrecord_create(
) {
    THIS( nullptr );
    DBG( "Creating xrecord listener…" );
    return new tapper::listener::xrecord_t;
};

namespace tapper {
namespace listener {

// -------------------------------------------------------------------------------------------------
// xrecord_t
// -------------------------------------------------------------------------------------------------

xrecord_t::xrecord_t(
):
    OBJECT_T(),
    _context( std::bind( & xrecord_t::on_intercept, this, std::placeholders::_1 ) )
{
}; // ctor

/** Returns `"XRecord"`. **/
string_t
xrecord_t::type(
) {
    return "XRecord";
}; // type

key_t::range_t
xrecord_t::key_range(
) {
    if ( not _key_range ) {
        /*
            Get Xkb key range.
        */
        auto xkb_range = kb_desc().key_range();
        // Convert Xkb key range to Tapper key range:
        auto range = key_t::range_t(
            x::key_t( xkb_range.min ).linux().code(),
            x::key_t( xkb_range.max ).linux().code()
        );
        DBG( "XRecord interceptor key range: " << range << " (keys only)." );
        /*
            Extend key range with button codes. X button codes are sequential from `x::button_min`
            to `x::button_max`, but corresponding Linux key codes may or may not be sequential, so
            range `key_range_t( x::button( x::button_min ), x::button( x::button_max ) )` may be
            invalid. Let's add buttons one-by-one.
        */
        for ( size_t code = x::btn_t::min; code <= x::btn_t::max; ++ code ) {
            auto key = x::btn_t( code ).linux();
            if ( key.code() ) {
                range += key.code();
            };
        };
        DBG( "XRecord interceptor key range: " << range << " (keys and buttons)." );
        assert( range.is_valid() );
        _key_range.reset( new key_t::range_t( range ) );
    };
    return * _key_range.get();
}; // key_range

keys_t
xrecord_t::keys(
) {
    keys_t keys;
    for ( auto key: key2name() ) {
        keys.insert( key.first );
    };
    return keys;
};

key_t
xrecord_t::key(
    string_t const & name
) {
    auto & map = name2key();
    auto it = map.find( uc( name ) );
    if ( it == map.end() ) {
        return key_t();         // TODO: Exception?
    };
    return it->second;
}; // key

string_t
xrecord_t::key_name(
    key_t key
) {
    key_range().check( key );
    auto const & map = key2name();
    auto it = map.find( key );
    if ( it == map.end() ) {
        return "";
    };
    return it->second;
}; // key_name

strings_t
xrecord_t::key_names(
    key_t key
) {
    auto name = key_name( key );
    return name.empty() ? strings_t{} : strings_t{ name };
}; // key_names

void
xrecord_t::_start(
) {
    _context.enable();
}; // _start

void
xrecord_t::_stop(
) {
    _context.disable();
}; // _stop

x::kb_t &
xrecord_t::kb(
) {
    if ( not _kb ) {
        _kb.reset( new x::kb_t( _context.display() ) );
    };
    return * _kb.get();
}; // kb

x::kb_t::desc_t &
xrecord_t::kb_desc(
) {
    if ( not _kb_desc ) {
        _kb_desc = kb().desc();
    };
    return * _kb_desc.get();
}; // kb_desc

strings_t const &
xrecord_t::key_names(
) {
    if ( not _key_names ) {
        _key_names.reset( new strings_t( kb_desc().key_names() ) );
    };
    return * _key_names.get();
}; // key_names

xrecord_t::key2name_t const &
xrecord_t::key2name(
) {
    if ( not _key2name ) {
        key2name_t map;
        auto const & names = key_names();
        for ( size_t i = 0, end = names.size(); i < end; ++ i ) {
            auto const & name = names[ i ];
            if ( not name.empty() ) {
                auto key = x::key_t( i ).linux();
                map.insert( { key, name } );
                // Indices are unique, so there is no need to check that insertion was successful.
            };
        };
        for ( auto code = x::btn_t::min; code <= x::btn_t::max; ++ code ) {
            auto key = x::btn_t( code ).linux();
            if ( key.code() ) {
                auto name = STR( "BTN" << code );
                auto ok = map.insert( { key, name } ).second;
                assert( ok );   // Just in case.
                static_cast< void >( ok );
            };
        };
        _key2name.reset( new key2name_t( std::move( map ) ) );
    };
    return * _key2name.get();
}; // key2name

xrecord_t::name2key_t const &
xrecord_t::name2key(
) {
    if ( not _name2key ) {
        name2key_t map;
        for ( auto const & it: key2name() ) {
            auto ok = map.insert( { it.second, it.first } ).second;
            if ( not ok ) {
                auto name = it.second;
                auto k1   = map[ it.second ];
                auto k2   = it.first;
                WRN(
                    "X Window System reported non-unique key name " << q( name ) << " "
                        "for keys " << k1 << " and " << k2 << "; "
                        "the first code will be used for this key name."
                );
            };
        };
        _name2key.reset( new name2key_t( map ) );
    };
    return * _name2key.get();
}; // name2key

void
xrecord_t::on_intercept(
    XRecordInterceptData const * data
) {
    TRACE();
    if ( data->category == XRecordFromServer ) {

        /*
            Decoding data described in:
            http://www.x.org/releases/X11R7.7/doc/xproto/x11protocol.html#events:input
        */
        if ( data->data_len * 4 < sizeof( xEvent ) ) {
            // `data_len` is size of data in 4-byte units.
            WRN( "Intercepted data is too short." );
            return;
        }; // if
        auto event = reinterpret_cast< xEvent const * >( data->data );
        auto get_x_event_type = [ event ] () {
            auto type = event->u.u.type;
            STATIC_ASSERT( sizeof( type ) == 1 );   // No need to swap bytes.
            return type;
        };
        auto get_x_event_key = [ event ] () {
            auto key = event->u.u.detail;
            STATIC_ASSERT( sizeof( key ) == 1 );    // No need to swap bytes.
            return x::key_t( key );
        };
        auto get_x_event_button = [ event ] () {
            auto btn = event->u.u.detail;
            STATIC_ASSERT( sizeof( btn ) == 1 );    // No need to swap bytes.
            return x::btn_t( btn );
        };
        auto get_x_event_time = [ this, data, event ] () {
            auto time = event->u.keyButtonPointer.time;
            if ( data->client_swapped ) {
                WRN( "Swapping bytes…" );            // TODO: Warn once?
                STATIC_ASSERT( sizeof( time ) == 4 );
                time = bswap_32( time );
            };
            return time;
        };

        auto type = get_x_event_type();
        // My experiments show that data->server_time == event->u.keyButtonPointer.time.
        switch ( type ) {
            case KeyPress:
            case KeyRelease: {
                auto key = get_x_event_key().linux();
                _on_event( {
                    .time  = get_x_event_time(),
                    .key   = key,
                    .state = key_state( type == KeyPress ),
                } );
            } break;
            case ButtonPress:
            case ButtonRelease: {
                /*
                    X Window System reports mouse wheel rotation as mouse buttons 4, 5, 6, and 7
                    press and release events. For these events `linux()` method returns key with
                    zero code, so I have to make sure the code is not zero before calling
                    `_on_event()`.
                */
                auto key = get_x_event_button().linux();
                if ( key.code() ) {
                    _on_event( {
                        .time  = get_x_event_time(),
                        .key   = key,
                        .state = key_state( type == ButtonPress ),
                    } );
                };
            } break;
        }; // switch

    }; // if
}; // on_intercept

}; // namespace listener
}; // namespace tapper

// end of file //
