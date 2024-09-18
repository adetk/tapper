/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/libinput.hpp

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
    libinput wrappers interface.

    @sa libinput.cpp
**/

#ifndef _TAPPER_LIBINPUT_HPP_
#define _TAPPER_LIBINPUT_HPP_

#include "base.hpp"

#include <functional>
#include <map>

#include <libinput.h>

#include "linux.hpp"
#include "listener.hpp"
#include "posix.hpp"
#include "types.hpp"

struct udev;

namespace tapper {
/// libinput wrappers.
namespace libinput {

    // ---------------------------------------------------------------------------------------------
    // error_t
    // ---------------------------------------------------------------------------------------------

    class error_t: public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
    }; // class error_t

    // ---------------------------------------------------------------------------------------------
    // udev_t
    // ---------------------------------------------------------------------------------------------

    /**
        Wrapper for `udev` opaque handle. Calls `udev_ref` and `udev_unref` properly to avoid
        handle leaks.
    **/
    class udev_t {

        public:

            using myself_t = udev_t;

            udev_t();
            udev_t( myself_t const & that );
            ~udev_t();
            myself_t & operator =( myself_t const & that );

        public:

            udev * rep { nullptr };

    }; // class udev_t

    class context_t: public object_t {

        public:

            class event_t;

            /**
                Keyborad event. Includes time, key (Linux input event code), and key state (presses
                or released).
            **/
            class keyboard_event_t {
                friend class event_t;
                public:
                    time_t      time()  const;
                    key_t       key()   const;
                    key_state_t state() const;
                private:
                    explicit keyboard_event_t( libinput_event_keyboard * rep );
                    libinput_event_keyboard * _rep { nullptr };
            };

            /**
                Pointer (i. e. mouse) event. Includes time, button (Linux input event code), and
                button state (presses or released).
            **/
            class pointer_event_t {
                friend class event_t;
                public:
                    time_t      time()   const;
                    key_t       button() const;
                    key_state_t state()  const;
                private:
                    explicit pointer_event_t( libinput_event_pointer * rep );
                    libinput_event_pointer * _rep { nullptr };
            };

            class event_t: public object_t {
                friend class context_t;
                public:
                    /**
                        Only types used by Tapper are represented here. libinput has mamy more
                        event types, but they are out of Tapper interest.
                    **/
                    enum class type_t {
                        none           = LIBINPUT_EVENT_NONE,
                        keyboard_key   = LIBINPUT_EVENT_KEYBOARD_KEY,
                        pointer_button = LIBINPUT_EVENT_POINTER_BUTTON,
                    };
                    type_t              type()     const;
                    keyboard_event_t    keyboard() const;
                    pointer_event_t     pointer()  const;
                    time_t              time()     const;
                private:
                    explicit event_t( context_t & context );
                    ~event_t();
                private:
                    libinput_event * _rep;
            }; // class event_t

            using on_event_t = std::function< void( event_t const & ) >;

        public:

            context_t(
                on_event_t          on_event,
                string_t const &    seat,
                udev_t const &      udev = udev_t()
            );
            virtual ~context_t();
            void enable();
            void disable();
            int fd();
            virtual int  open( string_t const & path, int flags );
            virtual void close( int fd );

        private:

            ::libinput *  _rep { nullptr };

        private:

            enum class state_t {
                inited,
                enabled,
                disabled
            }; // enum state_t

            class thread_t: public posix::thread_t {
                using parent_t = posix::thread_t;
                friend class context_t;
                private:
                    explicit thread_t( context_t & context );
                    virtual void body() override;
                private:
                    context_t & _context;
            };

            using files_t = std::map< int, ptr_t< posix::file_t > >;

        private:

            on_event_t  _on_event;
            state_t     _state { state_t::inited };
            string_t    _seat;
            thread_t    _thread;
            files_t     _files;

    }; // class context_t

}; // namespace libinput

string_t str( libinput::context_t::event_t::type_t value );

}; // namespace tapper

#endif // _TAPPER_LIBINPUT_HPP_

// end of file //
