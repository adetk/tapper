/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/emitter-libevdev.cpp

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
    `emitter::libevdev_t` class implementation.
**/

#include "emitter-libevdev.hpp"
#include "emitter-libevdev.h"

#include <limits>

tapper::emitter_t *
emitter_libevdev_create(
) {
    THIS( nullptr );
    DBG( "Creating libevdev emitter…" );
    return new tapper::emitter::libevdev_t;
};

namespace tapper {
namespace emitter {

// -------------------------------------------------------------------------------------------------
// libevdev_t
// -------------------------------------------------------------------------------------------------

libevdev_t::libevdev_t(
):
    OBJECT_T()
{
}; // ctor

string_t
libevdev_t::type(
) {
    return "libevdev";
}; // type

void
libevdev_t::start(
    keys_t const & keys
) {
    DBG( "Starting libevdev emitter…" );
    for ( auto key: keys ) {
        _keys.insert( key );
    };
    libevdev::evdev_t evdev;
    evdev.set_name( "Tapper virtual keyboard" );
    //~ evdev.set_id_bustype( BUS_USB );
    /*
        List of known USB ids: <http://www.linux-usb.org/usb.ids>.

        Looks like ACED is not yet used as a vendor id. BABE is used as product id, but with
        different vendor id.
    */
    evdev.set_id_vendor( 0xACED );
    evdev.set_id_product( 0xBABE );
    evdev.enable_event_type( linux::ev_key );
    for ( auto key: _keys ) {
        evdev.enable_event_code( linux::ev_key, key.code() );
    };
    _uinput.reset( new libevdev::uinput_t( evdev ) );
}; // start

void
libevdev_t::stop(
) {
    _uinput.reset();
}; // stop

void
libevdev_t::emit(
    events_t const & events
) {
    for (auto & event: events) {
        if ( event.key.code() ) {
            DBG(
                "Emitting " << event.key << " " <<
                    ( event.state == key_state_t::pressed ? "press" : "release" ) << "…"
            );
            _uinput->write_event(
                linux::ev_key, event.key.code(), event.state == key_state_t::pressed
            );
        };
    };
    _uinput->write_event( linux::ev_syn, linux::syn_report, 0 );
}; // emit

}; // namespace emitter
}; // namespace tapper

// end of file //
