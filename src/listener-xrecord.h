/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/listener-xrecord.h

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
    C interface to XRecord listener factory.
**/

#ifndef _TAPPER_LISTENER_XRECORD_H_
#define _TAPPER_LISTENER_XRECORD_H_

#include "listener.hpp"

extern "C" {
    tapper::listener_t * listener_xrecord_create();
}; // extern "C"

#endif // _TAPPER_LISTENER_XRECORD_H_

// end of file //
