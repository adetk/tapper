/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/emitter.hpp

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
    `emitter_t` abstract class interface.

    @sa emitter.cpp
**/

#ifndef _TAPPER_EMITTER_HPP_
#define _TAPPER_EMITTER_HPP_

#include "base.hpp"

#include "settings.hpp"

namespace tapper {

// -------------------------------------------------------------------------------------------------
// emitter_t
// -------------------------------------------------------------------------------------------------

class emitter_t;
typedef ptr_t< emitter_t > emitter_p;

/**
    Emitter abstract interface and factory.

    Emitter is a component which emits input events (i. e. simulates keystrokes).
**/
class emitter_t {

    public:

        /** Emitter exceptions. **/
        class error_t: public std::runtime_error {
            public:
                using std::runtime_error::runtime_error;
        }; // class error_t

        struct event_t {
            key_t       key;
            key_state_t state;
        };
        using events_t = std::vector< event_t >;

    public:

        /** Emitter factory. Returns pointer to a new emitter of the specified type. **/
        static emitter_t * create( settings_t::emitter_t type );

        virtual ~emitter_t() = default;

        /** Returns the emitter type name. **/
        virtual string_t type() = 0;

        /** Starts the emitter. **/
        virtual void start( keys_t const & keys ) = 0;

        /**
            Emit the given series of input events. The function should not be called before `start`
            and after `stop`.
        **/
        virtual void emit( events_t const & event ) = 0;

        /** Stops emitter. **/
        virtual void stop() = 0;

}; // class emitter_t

/// Namespace for concrete emitters.
namespace emitter {
}; // namespace emitter

}; // namespace tapper

#endif // _TAPPER_EMITTER_HPP_

// end of file //
