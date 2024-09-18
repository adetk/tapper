/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter-dummy.cpp

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
    `layouter::dummy_t` class implementation.

    @sa layouter-dummy.h
    @sa layouter-dummy.hpp
**/

#include "layouter-dummy.hpp"
#include "layouter-dummy.h"

tapper::layouter_t *
layouter_dummy_create(
) {
    THIS( nullptr );
    DBG( "Creating dummy layouter…" );
    return new tapper::layouter::dummy_t;
};

namespace tapper {
namespace layouter {

// -------------------------------------------------------------------------------------------------
// dummy_t
// -------------------------------------------------------------------------------------------------

dummy_t::dummy_t(
):
    OBJECT_T()
{
}; // ctor

/** Returms `"Dummy"`. **/
string_t
dummy_t::type(
) {
    return "Dummy";
}; // type

/** Prints debug message, *does not* activate layout. **/
void
dummy_t::activate(
    layout_t layout
) {
    INF( "(Activate layout " << layout << ".)" );
}; // activate

}; // namespace layouter
}; // namespace tapper

// end of file //
