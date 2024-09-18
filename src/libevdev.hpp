/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/libevdev.hpp

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
    libevdev wrappers interface.

    @sa libevdev.cpp
**/

#ifndef _TAPPER_LIBEVDEV_HPP_
#define _TAPPER_LIBEVDEV_HPP_

#include "base.hpp"

#include <functional>
#include <initializer_list>
#include <map>

#include "posix.hpp"
#include "linux.hpp"

struct libevdev;
struct libevdev_uinput;

namespace tapper {
/** libevdev wrappers. **/
namespace libevdev {

    using event_type_t = uint_t;
    using event_code_t = uint_t;

    // ---------------------------------------------------------------------------------------------
    // error_t
    // ---------------------------------------------------------------------------------------------

    class error_t: public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
    }; // class error_t

    // ---------------------------------------------------------------------------------------------
    // evdev_t
    // ---------------------------------------------------------------------------------------------

    /**
        Wrapper for `libevdev *` opaque handler. Calls `libevdev_new` and `libevdev_free` properly
        to avoid handle leaks.
    **/
    class evdev_t {

        friend class uinput_t;

        public:

            typedef evdev_t         myself_t;

            evdev_t();
            evdev_t( myself_t const & that ) = delete;
            ~evdev_t();

            myself_t & operator =( myself_t const & that ) = delete;

            void set_name( string_t const & name );
            void set_id_bustype( int id );
            void set_id_vendor( int id );
            void set_id_product( int id );
            void set_id_version( int id );
            void enable_event_type( event_type_t type );
            void enable_event_code( event_type_t type, event_code_t code );

        private:

            ::libevdev * _rep { nullptr };

    }; // class evdev_t

    // ---------------------------------------------------------------------------------------------
    // uinput_t
    // ---------------------------------------------------------------------------------------------

    /**
        Wrapper for `libevdev_uinput *` opaque handle.
    **/
    class uinput_t {

        public:

            typedef uinput_t myself_t;

            explicit uinput_t( evdev_t const & evdev );
            uinput_t( myself_t const & that ) = delete;
            uinput_t( myself_t && that );
            ~uinput_t();

            myself_t & operator =( myself_t const & that ) = delete;

            string_t get_devnode();
            string_t get_syspath();

            void write_event( event_type_t type, event_code_t code, int value );

        private:

            posix::file_t       _file;
            ::libevdev_uinput * _rep { nullptr };

    }; // class uinput_t

    using uinput_p = ptr_t< uinput_t >;

}; // namespace libevdev
}; // namespace tapper

#endif // _TAPPER_LIBEVDEV_HPP_

// end of file //
