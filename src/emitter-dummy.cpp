/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/emitter-dummy.cpp

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
    `emitter::dummy_t` class implementation.
**/

#include "emitter-dummy.hpp"
#include "emitter-dummy.h"

tapper::emitter_t *
emitter_dummy_create(
) {
    THIS( nullptr );
    DBG( "Creating dummy emitter…" );
    return new tapper::emitter::dummy_t;
};

namespace tapper {
namespace emitter {

// -------------------------------------------------------------------------------------------------
// dummy_t
// -------------------------------------------------------------------------------------------------

dummy_t::dummy_t(
):
    OBJECT_T()
{
}; // ctor

string_t
dummy_t::type(
) {
    return "Dummy";
}; // type

void
dummy_t::start(
    keys_t const &
) {
}; // start

void
dummy_t::stop(
) {
}; // stop

void
dummy_t::emit(
    events_t const & events
) {
    for (auto & event: events) {
        switch ( event.state ) {
            case key_state_t::pressed: {
                INF( "(Emitting " << event.key << " press…)" );
            } break;
            case key_state_t::released: {
                INF( "(Emitting " << event.key << " release…)" );
            } break;
        };
    };
}; // emit


}; // namespace emitter
}; // namespace tapper

// end of file //
