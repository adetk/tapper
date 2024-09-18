/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/timer.cpp

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
    `timer_t` class implementation.
**/

#include "timer.hpp"

namespace tapper {

timer_t::timer_t(
    string_t const & action
):
    _action( action ),
    _started( clock_t::now() )
{
};

timer_t::~timer_t(
) {
    using seconds_t = std::chrono::duration< double >;
    auto elapsed = ( _stopped == point_t() ? clock_t::now() : _stopped ) - _started;
    INF( _action << ": " << seconds_t( elapsed ).count() << " s." );
};

void
timer_t::restart(
) {
    _started = clock_t::now();
};

void
timer_t::stop(
) {
    if ( _stopped != point_t() ) {
        assert( 0 );
    };
    _stopped = clock_t::now();
};

}; // namespace tapper

// end of file //
