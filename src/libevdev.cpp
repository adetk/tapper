/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/libevdev.cpp

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
    libevdev wrappers implementation.
**/

#include "libevdev.hpp"

#include <system_error>
#include <utility>

#include <unistd.h>

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#include "posix.hpp"
#include "privileges.hpp"
#include "string.hpp"

namespace tapper {
namespace libevdev {

// -------------------------------------------------------------------------------------------------
// class evdev_t
// -------------------------------------------------------------------------------------------------

evdev_t::evdev_t(
):
    _rep( libevdev_new() )
{
    if ( not _rep ) {
        ERR( "Failed to initialize evdev device (looks like no memory)." );
    };
};

evdev_t::~evdev_t(
) {
    libevdev_free( _rep );
    _rep = nullptr;
};

void
evdev_t::set_name(
    string_t const & name
) {
    libevdev_set_name( _rep, name.c_str() );
};

void
evdev_t::set_id_bustype(
    int id
) {
    libevdev_set_id_bustype( _rep, id );
};

void
evdev_t::set_id_vendor(
    int id
) {
    libevdev_set_id_vendor( _rep, id );
};

void
evdev_t::set_id_product(
    int id
) {
    libevdev_set_id_product( _rep, id );
};

void
evdev_t::set_id_version(
    int id
) {
    libevdev_set_id_version( _rep, id );
};

void
evdev_t::enable_event_type(
    event_type_t type
) {
    auto err = libevdev_enable_event_type( _rep, type );
    if ( err ) {
        // errno is not set.
        ERR( "Can't enable event type " << type << "." );
    };
};

void
evdev_t::enable_event_code(
    event_type_t type,
    event_code_t code
) {
    auto err = libevdev_enable_event_code( _rep, type, code, nullptr );
    if ( err ) {
        // errno is not set.
        ERR( "Can't enable event code ( " << type << ", " << code << " )." );
    };
};

// -------------------------------------------------------------------------------------------------
// class uinput_t
// -------------------------------------------------------------------------------------------------

uinput_t::uinput_t(
    evdev_t const & evdev
) {
    privileges().do_as_root( [ this ] () {
        _file.open( "/dev/uinput", O_RDWR );
    } );
    auto err = libevdev_uinput_create_from_device( evdev._rep, _file.fd(), & _rep );
    if ( err ) {
        ERR( "Can't create uinput device: " << posix::syserrmsg( - err ) );
    };
    DBG( "Uinput device " << q( get_devnode() ) << " (" << q( get_syspath() ) << ") created." );
};

uinput_t::uinput_t(
    myself_t && that
) : _rep( nullptr ) {
    std::swap( _rep, that._rep );
};

uinput_t::~uinput_t(
) {
    libevdev_uinput_destroy( _rep );
    _rep = nullptr;
};

string_t
uinput_t::get_devnode(
) {
    auto devnode = libevdev_uinput_get_devnode( _rep );
    if ( devnode == nullptr ) {
        ERR( "Can't get device node for the uinput device." );
    };
    return devnode;
};

string_t
uinput_t::get_syspath(
) {
    auto syspath = libevdev_uinput_get_syspath( _rep );
    if ( syspath == nullptr ) {
        ERR( "Can't get syspath for the uinput device." );
    };
    return syspath;
};

void
uinput_t::write_event(
    event_type_t    type,
    event_code_t    code,
    int             value
) {
    auto err = libevdev_uinput_write_event( _rep, type, code, value );
    if ( err ) {
        // err is negative errno.
        ERR( "Can't emit input event: " << posix::syserrmsg( - err ) );
    };
};

}; // namespace libevdev
}; // namespace tapper

// end of file //
