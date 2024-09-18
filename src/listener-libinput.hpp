/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/listener-libinput.hpp

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
    `listener::libinput_t` class interface.

    @sa listener-libinput.cpp
**/

#ifndef _TAPPER_LISTENER_LIBINPUT_HPP_
#define _TAPPER_LISTENER_LIBINPUT_HPP_

#include "listener.hpp"

#include "libinput.hpp"

namespace tapper {
namespace listener {

// -------------------------------------------------------------------------------------------------
// libinput_t
// -------------------------------------------------------------------------------------------------

/**
    libinput listener. libinput is low-level library to handle input devices. The libinput listener
    uses this library and works for both X Window System and Wayland, but requires extra
    permissions: the Tapper process must belong to the '`input`' group. (It could be achieved if
    '`tapper`' program file belongs to the '`input`' group and has the '`set-group-id`' access flag
    set. This is the reason why Tapper should be installed by root.)
**/
class libinput_t: public object_t, public listener_t {

    public:

        explicit libinput_t();

        virtual string_t       type()                       override;
        virtual key_t::range_t key_range()                  override;
        virtual keys_t         keys()                       override;
        virtual key_t          key( string_t const & name ) override;
        virtual string_t       key_name( key_t key )        override;
        virtual strings_t      key_names( key_t key )       override;

    protected:

        virtual void           _start()                     override;
        virtual void           _stop()                      override;

    private:

        void _on_intercept( libinput::context_t::event_t const & event );

        libinput::context_t         _context;

}; // class libinput_t

}; // namespace listener
}; // namespace tapper

#endif // _TAPPER_LISTENER_LIBINPUT_HPP_

// end of file //
