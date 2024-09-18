/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/emitter.cpp

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
    `emitter_t` abstract class implementation.
**/

#include "emitter.hpp"

#include "emitter-dummy.h"
#if WITH_LIBEVDEV
    #include "emitter-libevdev.h"
#endif // WITH_LIBEVDEV
#if WITH_X
    #include "emitter-xtest.h"
#endif // WITH_X

namespace tapper {

// -------------------------------------------------------------------------------------------------
// emitter_t
// -------------------------------------------------------------------------------------------------

emitter_t *
emitter_t::create(
    settings_t::emitter_t type
) {
    emitter_t * emitter = nullptr;
    switch ( type ) {
        case settings_t::emitter_t::unset: {
            assert( 0 );
        } break;
        case settings_t::emitter_t::Auto: {
            assert( 0 );
        } break;
        case settings_t::emitter_t::dummy: {
            emitter = emitter_dummy_create();
        } break;
        case settings_t::emitter_t::libevdev: {
            #if WITH_LIBEVDEV
                emitter = emitter_libevdev_create();
            #endif // WITH_LIBEVDEV
        } break;
        case settings_t::emitter_t::xtest: {
            #if WITH_X
                emitter = emitter_xtest_create();
            #endif // WITH_X
        } break;
    };
    if ( not emitter ) {
        ERR( "No emitters available." );
    };
    return emitter;
};

}; // namespace tapper

// end of file //
