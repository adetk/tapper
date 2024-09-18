/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter-kde.hpp

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
    `layouter::kde_t` class interface.

    @sa layouter-kde.h
    @sa layouter-kde.cpp
**/

#ifndef _TAPPER_LAYOUTER_KDE_HPP_
#define _TAPPER_LAYOUTER_KDE_HPP_

#include "base.hpp"

#include "dbus.hpp"
#include "layouter.hpp"

namespace tapper {
namespace layouter {

// -------------------------------------------------------------------------------------------------
// kde_t
// -------------------------------------------------------------------------------------------------

/**
    KDE layouter.

    Implements `layouter_t` interface. This layouter reports KDE keyboard layouts and activates
    them by invoking KDE methods via D-BUS.

    Obviously this layouter requires KDE.
**/
class kde_t: public object_t, public layouter_t {

    public:

        explicit               kde_t();
        virtual string_t       type()                          override;
        virtual void           activate( layout_t layout )     override;

    protected:

        virtual time_t         _get_repeat_delay()             override;
        virtual strings_t      _get_layout_names()             override;

    private:

        dbus_t  _dbus;

}; // class kde_t

}; // namespace layouter
}; // namespace tapper

#endif // _TAPPER_LAYOUTER_KDE_HPP_

// end of file //
