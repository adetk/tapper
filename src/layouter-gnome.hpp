/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter-gnome.hpp

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
    `layouter::gnome_t` class interface.

    @sa layouter-gnome.h
    @sa layouter-gnome.cpp
**/

#ifndef _TAPPER_LAYOUTER_GNOME_HPP_
#define _TAPPER_LAYOUTER_GNOME_HPP_

#include "base.hpp"

#include "dbus.hpp"
#include "layouter.hpp"

namespace tapper {
namespace layouter {

// -------------------------------------------------------------------------------------------------
// gnome_t
// -------------------------------------------------------------------------------------------------

/**
    GNOME layouter.

    Implements `layouter_t` interface. This layouter reports GNOME Shell 'input sources' and
    activates them by invoking GNOME Shell methods via D-BUS.

    Obviously this layouter requires GNOME Shell. It works for GNOME desktop, GNOME Classic desktop
    and Ubuntu desktop, and is not suitable for non-GNOME desktops.
**/
class gnome_t: public object_t, public layouter_t {

    public:

        explicit          gnome_t();
        virtual string_t  type()                      override;
        virtual void      activate( layout_t layout ) override;

    protected:

        virtual time_t    _get_repeat_delay()         override;
        virtual strings_t _get_layout_names()         override;
        virtual bool      _can_ring()                 override;
        virtual void      _start()                    override;
        virtual void      _stop()                     override;

    private:

        dbus_t          _dbus;
        gint            _id;
        gint            _sub;

}; // class gnome_t

}; // namespace layouter
}; // namespace tapper

#endif // _TAPPER_LAYOUTER_GNOME_HPP_

// end of file //
