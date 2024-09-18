/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/libinput.cpp

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
    libinput wrappers implementation.
**/

#include "libinput.hpp"

#include <fcntl.h>
#include <libinput.h>
#include <libudev.h>
#include <poll.h>
#include <unistd.h>

#include "privileges.hpp"
#include "string.hpp"
#include "types.hpp"

namespace tapper {
namespace libinput {

// -------------------------------------------------------------------------------------------------
// class udev_t
// -------------------------------------------------------------------------------------------------

udev_t::udev_t(
):
    rep( udev_new() )
{
    if ( not rep ) {
        ERR( "Failed to initialize udev" );
    };
};

udev_t::udev_t(
    myself_t const & that
):
    rep( that.rep )
{
    udev_ref( rep );
};

udev_t::~udev_t(
) {
    udev_unref( rep );
    rep = nullptr;
};

udev_t::myself_t &
udev_t::operator =(
    myself_t const & that
) {
    udev_unref( rep );
    rep = that.rep;
    udev_ref( rep );
    return * this;
};

// -------------------------------------------------------------------------------------------------
// class context_t
// -------------------------------------------------------------------------------------------------

static
int
_open(
    char const *    path,
    int             flags,
    void *          data
) {
    return reinterpret_cast< context_t * >( data )->open( path, flags );
};

static
void
_close(
    int             fd,
    void *          data
) {
    return reinterpret_cast< context_t * >( data )->close( fd );
};

static
void _log_handler(
    struct libinput *           libinput,
    enum libinput_log_priority  priority,
    const char *                format,
    va_list                     args
) {
    char buffer[ 1024 ];
    // TODO: rprintf
    vsnprintf( buffer, sizeof( buffer ), format, args );
    auto msg = chop( buffer, "\n" );
    switch ( priority ) {
        case LIBINPUT_LOG_PRIORITY_DEBUG: {
            DBG( "libinput: #: " << msg );
        } break;
        case LIBINPUT_LOG_PRIORITY_INFO: {
            /*
                Informational messages from libinput are not very interesting: they report every
                input device.
            */
            DBG( "libinput: I: " << msg );
        } break;
        case LIBINPUT_LOG_PRIORITY_ERROR: {
            WRN( "libinput: E: " << msg );
        } break;
    };
};

static const libinput_interface _interface = { _open, _close };

context_t::context_t(
    on_event_t          on_event,
    string_t const &    seat,
    udev_t const &      udev
):
    OBJECT_T(),
    _rep( libinput_udev_create_context( & _interface, this, udev.rep ) ),
    _on_event( on_event ),
    _seat( seat ),
    _thread( * this )
{
    if ( not _rep ) {
        ERR( "Failed to initialize libinput context." );
    };
    libinput_log_set_handler( _rep, & _log_handler );
    libinput_log_set_priority(
        _rep,
        #if ENABLE_DEBUG
            LIBINPUT_LOG_PRIORITY_DEBUG
        #else
            LIBINPUT_LOG_PRIORITY_INFO
        #endif
    );
};

context_t::~context_t(
) {
    CATCH_ALL( disable() );
    libinput_unref( _rep );
};

void
context_t::enable(
) {
    int err = libinput_udev_assign_seat( _rep, _seat.c_str() );
    if ( err ) {
        ERR( "Failed to assign seat to libinput context." );
    };
    _thread.start();
    _state = state_t::enabled;
};

void
context_t::disable(
) {
    switch ( _state ) {
        case state_t::inited: {
            // Not enabled — nothing to do.
        } break;
        case state_t::enabled: {
            /*
                This code relies on the application (since signal disposition is process-wide):
                Application should handle SIGINT signal to let ppoll be interrupted.
            */
            _thread.kill( SIGINT );
            _thread.join();
            _state = state_t::disabled;
        } break;
        case state_t::disabled: {
            // Already disabled — nothing to do.
        } break;
    };
};

int
context_t::fd(
) {
    return libinput_get_fd( _rep );
};

int
context_t::open(
    string_t const &    path,
    int                 flags
) {
    auto file = ptr_t< posix::file_t >( new posix::file_t );
    privileges().do_as_input( [ & file, & path, flags ] () {
        file->open( path, flags );
    } );
    auto fd = file->fd();
    _files[ fd ] = std::move( file );
    return fd;
};

void
context_t::close(
    int fd
) {
    auto it = _files.find( fd );
    if ( it == _files.end() ) {
        WRN( "libinput requested to close unknown file descriptor " << fd << "." );
    } else {
        _files.erase( it );
    };
};

context_t::thread_t::thread_t(
    context_t & context
):
    parent_t( "lictx" ),
    _context( context )
{
};

void
context_t::thread_t::body(
) {
    /*
        In the very beginning there are some add_device events in the queue. However, poll does not
        sense them and will wait until the first "real" input event. It is not documented but
        Libinput debug tools (event-debug and list-devices) are implemented in such way.
    */
    struct pollfd pfd {
        .fd      = _context.fd(),
        .events  = POLLIN,
        .revents = 0,
    };
    for ( ; ; ) {
        for ( ; ; ) {
            int err = libinput_dispatch( _context._rep );
            if ( err ) {
                ERR( "Libinput dispatch failed: " << posix::syserrmsg( -err ) << "." );
            };
            event_t event( _context );
            auto type = event.type();
            if ( type == event_t::type_t::none ) {
                break;
            };
            _context._on_event( event );
        };
        int error = ::poll( & pfd, 1, -1 );
        if ( error < 0 ) {
            error = errno;
            if ( error == EINTR ) { // Interrupted system call.
                break;              // This is not an actual error, just exit the loop.
            };
            ERR( "Failed to poll fd #" << pfd.fd << ": " << posix::syserrmsg( error ) << "." );
        };
    };
};

// -------------------------------------------------------------------------------------------------
// keyboard_event_t
// -------------------------------------------------------------------------------------------------

time_t
context_t::keyboard_event_t::time(
) const {
    auto time = libinput_event_keyboard_get_time( _rep );
    STATIC_ASSERT( sizeof( time ) == sizeof( time_t ) );
    return time;
};

key_t
context_t::keyboard_event_t::key(
)  const {
    auto key = libinput_event_keyboard_get_key( _rep );
    STATIC_ASSERT( sizeof( key ) == sizeof( key_t ) );
    return key_t( key );
};

key_state_t
context_t::keyboard_event_t::state(
)  const {
    auto state = libinput_event_keyboard_get_key_state( _rep );
    switch ( state ) {
        case LIBINPUT_KEY_STATE_RELEASED: {
            return key_state_t::released;
        } break;
        case LIBINPUT_KEY_STATE_PRESSED: {
            return key_state_t::pressed;
        } break;
    };
    assert( 0 );
};

context_t::keyboard_event_t::keyboard_event_t(
    libinput_event_keyboard * rep
): _rep( rep ) {
};

// -------------------------------------------------------------------------------------------------
// pointer_event_t
// -------------------------------------------------------------------------------------------------

time_t
context_t::pointer_event_t::time(
) const {
    auto time = libinput_event_pointer_get_time( _rep );
    STATIC_ASSERT( sizeof( time ) == sizeof( time_t ) );
    return time;
};

key_t
context_t::pointer_event_t::button(
)  const {
    auto button = libinput_event_pointer_get_button( _rep );
    STATIC_ASSERT( sizeof( button ) == sizeof( key_t ) );
    return key_t( button );
};

key_state_t
context_t::pointer_event_t::state(
)  const {
    auto state = libinput_event_pointer_get_button_state( _rep );
    switch ( state ) {
        case LIBINPUT_BUTTON_STATE_RELEASED: {
            return key_state_t::released;
        } break;
        case LIBINPUT_BUTTON_STATE_PRESSED: {
            return key_state_t::pressed;
        } break;
    };
    assert( 0 );
};

context_t::pointer_event_t::pointer_event_t(
    libinput_event_pointer * rep
): _rep( rep ) {
};

// -------------------------------------------------------------------------------------------------
// event_t
// -------------------------------------------------------------------------------------------------

context_t::event_t::event_t(
    context_t & context
):
    OBJECT_T(),
    _rep( libinput_get_event( context._rep ) )
{
};

context_t::event_t::~event_t(
) {
    libinput_event_destroy( _rep );
};

context_t::event_t::type_t
context_t::event_t::type(
) const {
    if ( _rep ) {
        return type_t( libinput_event_get_type( _rep ) );
    } else {
        return type_t::none;
    };
};

context_t::keyboard_event_t
context_t::event_t::keyboard(
) const {
    assert( type() == type_t::keyboard_key );
    return keyboard_event_t( libinput_event_get_keyboard_event( _rep ) );
};

context_t::pointer_event_t
context_t::event_t::pointer(
) const {
    assert( type() == type_t::pointer_button );
    return pointer_event_t( libinput_event_get_pointer_event( _rep ) );
};

}; // namespace libinput

string_t
str(
    libinput::context_t::event_t::type_t  value
) {
    using type_t = libinput::context_t::event_t::type_t;
    static std::map< type_t, string_t > const map {
        #define EVENT( event )  { type_t( LIBINPUT_EVENT_ ## event ), lc( # event ) }
        EVENT( NONE                    ),
        EVENT( DEVICE_ADDED            ),
        EVENT( DEVICE_REMOVED          ),
        EVENT( KEYBOARD_KEY            ),
        EVENT( POINTER_MOTION          ),
        EVENT( POINTER_MOTION_ABSOLUTE ),
        EVENT( POINTER_BUTTON          ),
        EVENT( POINTER_AXIS            ),
        EVENT( TOUCH_DOWN              ),
        EVENT( TOUCH_UP                ),
        EVENT( TOUCH_MOTION            ),
        EVENT( TOUCH_CANCEL            ),
        EVENT( TOUCH_FRAME             ),
        EVENT( TABLET_TOOL_AXIS        ),
        EVENT( TABLET_TOOL_PROXIMITY   ),
        EVENT( TABLET_TOOL_TIP         ),
        EVENT( TABLET_TOOL_BUTTON      ),
        EVENT( TABLET_PAD_BUTTON       ),
        EVENT( TABLET_PAD_RING         ),
        EVENT( TABLET_PAD_STRIP        ),
        EVENT( GESTURE_SWIPE_BEGIN     ),
        EVENT( GESTURE_SWIPE_UPDATE    ),
        EVENT( GESTURE_SWIPE_END       ),
        EVENT( GESTURE_PINCH_BEGIN     ),
        EVENT( GESTURE_PINCH_UPDATE    ),
        EVENT( GESTURE_PINCH_END       ),
        #undef EVENT
    };
    auto it = map.find( value );
    if ( it == map.end() ) {
        return "(* unknown libinput event type #" + str( int_t( value ) ) + " *)";
    } else {
        return it->second;
    };
};

}; // namespace tapper

// end of file //
