/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/emitter-libevdev.hpp

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
    `emitter::libevdev_t` class interface.

    @sa emitter-libevdev.cpp
**/

#ifndef _TAPPER_EMITTER_LIBEVDEV_HPP_
#define _TAPPER_EMITTER_LIBEVDEV_HPP_

#include "base.hpp"
#include "emitter.hpp"

#include "libevdev.hpp"

namespace tapper {
namespace emitter {

// -------------------------------------------------------------------------------------------------
// libevdev_t
// -------------------------------------------------------------------------------------------------

/** libevdev emitter. **/
class libevdev_t: public object_t, public emitter_t {

    public:

        explicit         libevdev_t();
        virtual string_t type()                               override;
        virtual void     start( keys_t const & keys )         override;
        virtual void     emit( events_t const & events )      override;
        virtual void     stop()                               override;

    private:

        libevdev::uinput_p  _uinput;
        keys_t              _keys;

}; // class libevdev_t

}; // namespace emitter
}; // namespace tapper

#endif // _TAPPER_EMITTER_LIBEVDEV_HPP_

// end of file //
