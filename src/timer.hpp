/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/timer.hpp

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
    `timer_t` class interface.

    @sa timer.cpp
**/

#ifndef _TAPPER_TIMER_HPP_
#define _TAPPER_TIMER_HPP_

#include "base.hpp"

#include <chrono>

// TODO: --enable-timing configure option.
// TODO: Convenient macro to use timer conditionally.

namespace tapper {

class timer_t {

    public:

        explicit timer_t( string_t const & action );
        ~timer_t();

        void restart();
        void stop();

    private:

        using clock_t = std::chrono::high_resolution_clock;
        using point_t = std::chrono::time_point< clock_t >;

        string_t    _action;
        point_t     _started;
        point_t     _stopped;

}; // class timer_t

}; // namespace tapper

#endif // _TAPPER_TIMER_HPP_

// end of file //
