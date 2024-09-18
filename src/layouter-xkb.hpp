/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter-xkb.hpp

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
    `layouter::x_t` class interface.

    @sa layouter-xkb.cpp
**/

#ifndef _TAPPER_LAYOUTER_XKB_HPP_
#define _TAPPER_LAYOUTER_XKB_HPP_

#include "base.hpp"

#include "layouter.hpp"
#include "x.hpp"

namespace tapper {
namespace layouter {

// -------------------------------------------------------------------------------------------------
// x_t
// -------------------------------------------------------------------------------------------------

/**
    Xkb layouter.

    Implements `layouter_t` inteface. This layouter reports Xkb 'groups' (this is technical term
    used by X Window System for keyboard layout) and activates keyboard layouts by calling
    `XkbLockGroup` function.

    Obviously this layouter requires X Window System. This layouter works for non-GNOME desktops:
    LXDE, LXQt, Mate, Xfce, and probably others.
**/
class xkb_t: public object_t, public layouter_t {

    public:             // methods

        explicit               xkb_t();
        virtual string_t       type()                      override;

    protected:          // methods

        virtual layout_range_t layout_range()              override;
        virtual void           activate( layout_t layout ) override;
        virtual time_t         _get_repeat_delay()         override;
        virtual strings_t      _get_layout_names()         override;
        virtual bool           _can_ring()                 override;

    private:            // data

        x::display_t    _display;
        x::kb_t         _kb;

}; // class xkb_t

}; // namespace layouter
}; // namespace tapper

#endif // _TAPPER_LAYOUTER_XKB_HPP_

// end of file //
