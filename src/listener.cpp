/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/listener.cpp

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
    `listener_t` abstract class implementation.
**/

#include "listener.hpp"

#if WITH_LIBINPUT
    #include "listener-libinput.h"
#endif // WITH_LIBINPUT
#if WITH_X
    #include "listener-xrecord.h"
#endif // WITH_X

namespace tapper {

key_state_t
listener_t::key_state(
    bool pressed
) {
    return pressed ? key_state_t::pressed : key_state_t::released;
};

listener_t *
listener_t::create(
    settings_t::listener_t type
) {
    listener_t * listener = nullptr;
    switch ( type ) {
        case settings_t::listener_t::unset: {
            assert( 0 );
        } break;
        case settings_t::listener_t::Auto: {
            assert( 0 );
        } break;
        case settings_t::listener_t::libinput: {
            #if WITH_LIBINPUT
                listener = listener_libinput_create();
            #endif // WITH_LIBINPUT
        } break;
        case settings_t::listener_t::xrecord: {
            #if WITH_X
                listener = listener_xrecord_create();
            #endif // WITH_X
        } break;
    };
    if ( not listener ) {
        ERR( "No listeners available." );
    };
    return listener;
};

string_t
listener_t::key_full_name(
    key_t key
) {
    auto name = key_name( key );
    return str( key ) + ( name.empty() ? "" : ":" + name );
};

void
listener_t::start(
    on_event_t handler
) {
    _on_event = handler;
    _start();
};

void
listener_t::stop(
) {
    _stop();
    _on_event = nullptr;
};

}; // namespace tapper

// end of file //
