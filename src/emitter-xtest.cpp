/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/emitter-xtest.cpp

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
    `emitter::xtest_t` class implementation.
**/

#include "emitter-xtest.hpp"
#include "emitter-xtest.h"

#include "x.hpp"

tapper::emitter_t *
emitter_xtest_create(
) {
    THIS( nullptr );
    DBG( "Creating xtest emitter…" );
    return new tapper::emitter::xtest_t;
};

namespace tapper {
namespace emitter {

// -------------------------------------------------------------------------------------------------
// xtest_t
// -------------------------------------------------------------------------------------------------

xtest_t::xtest_t(
):
    OBJECT_T(),
    _test( _display )
{
}; // ctor

string_t
xtest_t::type(
) {
    return "XTest";
}; // type

void
xtest_t::start(
    keys_t const & keys
) {
    DBG( "Starting xtest emitter…" );
};

void
xtest_t::stop(
) {
};

void
xtest_t::emit(
    events_t const & events
) {
    for (auto & event: events) {
        if ( event.key.is_mouse_button() ) {
            auto code = x::btn_t( event.key ).code();
            if ( code ) {
                _test.fake_button_event( code, event.state == key_state_t::pressed );
            };
        } else {
            auto code = x::key_t( event.key ).code();
            if ( code ) {
                _test.fake_key_event( code, event.state == key_state_t::pressed );
            };
        };
    };
    _display.sync();
};

}; // namespace emitter
}; // namespace tapper

// end of file //
