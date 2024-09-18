/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter-xkb.cpp

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
    `layouter::x_t` class implementation.

    @sa layouter-kde.h
    @sa layouter-kde.hpp
**/

#include "layouter-xkb.hpp"
#include "layouter-xkb.h"

tapper::layouter_t *
layouter_xkb_create(
) {
    return new tapper::layouter::xkb_t;
};

namespace tapper {
namespace layouter {

// -------------------------------------------------------------------------------------------------
// xkb_t
// -------------------------------------------------------------------------------------------------

xkb_t::xkb_t(
):
    OBJECT_T(),
    _kb( _display )
{
}; // ctor

/** Returns `"Xkb"`. **/
string_t
xkb_t::type(
) {
    return "Xkb";
}; //type

layout_range_t
xkb_t::layout_range(
) {
    return layout_range_t( 1, x::layout_max );
};

time_t
xkb_t::_get_repeat_delay(
) {
    auto rate = _kb.autorepeat_rate();
    return rate.timeout;
};

strings_t
xkb_t::_get_layout_names(
) {
    return _kb.desc()->group_names();
};

void
xkb_t::activate(
    layout_t layout
) {
    assert( layout.index > 0 );
    _kb.lock_group( layout.index - 1 );
    if ( bell() ) {
        _kb.bell();
    }; // if
    _kb.display().flush();
}; // activate

bool
xkb_t::_can_ring(
) {
    return true;
};

}; // namespace layouter
}; // namespace tapper

// end of file //
