/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/listener.hpp

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
    `listener_t` abstract class interface.

    @sa listener.cpp
**/

#ifndef _TAPPER_LISTENER_HPP_
#define _TAPPER_LISTENER_HPP_

#include "base.hpp"

#include <functional>

#include "settings.hpp"

namespace tapper {

// -------------------------------------------------------------------------------------------------
// listener_t
// -------------------------------------------------------------------------------------------------

/**
    Listener abstract interface and factory.

    Listener is a component which listens for input (keyboard, mouse, etc) events and calls
    specified function, handler, on each input event.
**/
class listener_t {

    public:

        /** Listener exceptions. **/
        class error_t: public std::runtime_error {
            public:
                using std::runtime_error::runtime_error;
        }; // class error_t

        /**
            Converts boolean value to `key_state_t` value. `true` converted to
            `key_state_t::pressed`, `false` — to `key_state_t::released`.
        */
        key_state_t key_state( bool pressed );

        /** Keyboard event, either key (or button) press or release. **/
        struct event_t {
            time_t      time;   ///< Time when the event occurred, in milliseconds.
            key_t       key;    ///< Key which state was changed.
            key_state_t state;  ///< New state of the key.
        };

        /**
            Type of function called on every keyboard event.
        **/
        using on_event_t = std::function< void( event_t const & ) >;

    public:

        /** Listener factory. Returns pointer to a new listener of the specified type. **/
        static listener_t * create( settings_t::listener_t type );

        /** Virtual destructor, in order to provide polymorphism. **/
        virtual ~listener_t() = default;

        /** Returns the listener type name, e. g. "libinput" or "xrecord". **/
        virtual string_t type() = 0;

        /**
            Returns theoretical range of key codes. Not all keys in the range may actually exist.
        **/
        virtual key_t::range_t key_range() = 0;

        /** Returns set of keys whose names are known. **/
        virtual keys_t keys() = 0;

        /**
            Returns key by the given name. Listeners are free to implement case-sensitive or
            case-insensitive search, or search for abbreviated name (e. g. an listener may
            successfully search for "esc" while full key name could be "escape"). If no such key is
            found, 0 is returned.
        **/
        virtual key_t key( string_t const & name ) = 0;

        /**
            Returns name of the given key. If key is out of key range, `std::out_of_range` will be
            thrown. For keys in allowed range, name may be empty if key name is not known.
        **/
        virtual string_t key_name( key_t key ) = 0;

        /**
            Returns full name of the given key. Full name consists of key code, colon, and key
            name. `std::out_of_range` exception will be thrown if key is out of range. If key has
            no name, colon is not added.
        **/
        string_t key_full_name( key_t key );

        /**
            Returns all names (including aliases) of the given key. If key is out of key range,
            `std::out_of_range` will be thrown. For keys in allowed range, returned vector may be
            empty if the key has no names.
        **/
        virtual strings_t key_names( key_t key ) = 0;

        /**
            Starts listening.

            @param handler — The given function will be called on each user input event.
        **/
        void start( on_event_t handler );

        /** Stops listening, the handler function will not be called any more. **/
        void stop();

    protected:

        virtual void _start() = 0;
        virtual void _stop()  = 0;

        on_event_t  _on_event { nullptr };      ///< Function to call on every user input event.

}; // class listener_t

using listener_p = ptr_t< listener_t >;

/// Namespace for concrete listeners.
namespace listener {
}; // namespace listener

}; // namespace tapper

#endif // _TAPPER_LISTENER_HPP_

// end of file //
