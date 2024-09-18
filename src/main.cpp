/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/main.cpp

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
    `main` function is defined here.
**/

#include "base.hpp"

#include "app.hpp"

using namespace tapper;

int
main(
    int     argc,
    char *  argv[]
) {
    int status = app_t::ok;
    auto what = CATCH_ALL( app_t app( argc, argv ); );
    if ( not what.empty() ) {
        _guts::eprint( priority_t::error, what );
            // Don't use `ERR` macro here: it will throw an exception,
            // while I just want to print the error message.
        status = app_t::runtime_error;
    };
    return status;
}; // main

// end of file //
