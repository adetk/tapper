/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/listener-libinput.cpp

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
    `listener::libinput_t` class implementation.
**/

#include "listener-libinput.hpp"
#include "listener-libinput.h"

#include "linux.hpp"
#include "posix.hpp"

tapper::listener_t *
listener_libinput_create(
) {
    THIS( nullptr );
    DBG( "Creating libinput listener…" );
    return new tapper::listener::libinput_t;
};

namespace tapper {
namespace listener {

// -------------------------------------------------------------------------------------------------
// libinput_t
// -------------------------------------------------------------------------------------------------

libinput_t::libinput_t(
):
    OBJECT_T(),
    _context(
        std::bind( & libinput_t::_on_intercept, this, std::placeholders::_1 ),
        posix::get_env( "XDG_SEAT", "seat0" ) /*
            Some platforms define XDG_SEAT environment variable, it looks I should use it. If the
            variable is not defined, use default value "seat0".
        */
    )
{
}; // ctor

/** Returns `"libinput"`. **/
string_t
libinput_t::type(
) {
    return "libinput";
}; // type

key_t::range_t
libinput_t::key_range(
) {
    return linux::key_range();
}; // key_range

keys_t
libinput_t::keys(
) {
    return linux::keys();
};

key_t
libinput_t::key(
    string_t const & name
) {
    return linux::key( name );
}; // key

string_t
libinput_t::key_name(
    key_t key
) {
    return linux::key_name( key );
}; // key_name

strings_t
libinput_t::key_names(
    key_t key
) {
    return linux::key_names( key );
}; // key_names

void
libinput_t::_start(
) {
    DBG( "Starting libinput listener…" );
    _context.enable();
}; // _start

void
libinput_t::_stop(
) {
    DBG( "Stopping libinput listener…" );
    _context.disable();
}; // _stop

void
libinput_t::_on_intercept(
    libinput::context_t::event_t const & event
) {
    switch ( event.type() ) {
        case libinput::context_t::event_t::type_t::keyboard_key: {
            auto kbev  = event.keyboard();
            _on_event( {
                .time  = kbev.time(),
                .key   = kbev.key(),
                .state = kbev.state(),
            } );
        } break;
        case libinput::context_t::event_t::type_t::pointer_button: {
            auto ptev   = event.pointer();
            _on_event( {
                .time  = ptev.time(),
                .key   = ptev.button(),
                .state = ptev.state(),
            } );
        } break;
        default: {
            // Do nothing.
        } break;
    };
}; // on_intercept

}; // namespace listener
}; // namespace tapper

// end of file //
