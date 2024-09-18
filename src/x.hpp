/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/x.hpp

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
    X Window System wrappers interface.

    @sa x.cpp
**/

#ifndef _TAPPER_X_HPP_
#define _TAPPER_X_HPP_

#include "base.hpp"

#include <functional>
#include <stdexcept>        // std::runtime_error

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#include <X11/extensions/record.h>
#include <X11/extensions/XKB.h>

#include "linux.hpp"
#include "posix.hpp"

namespace tapper {
/// X Window System wrappers.
namespace x {

    static uint_t constexpr layout_max = XkbNumKbdGroups;

    // ---------------------------------------------------------------------------------------------
    // key_t
    // ---------------------------------------------------------------------------------------------

    /**
        X Window System key code. Xkb usually reports key range as `[8…255]`, but key with code 8
        does not have name and corresponds to Linux key code 0, which is reserved. However, it
        looks like X Window System does not have troubles with key code 8. For example, `xdotool`
        uses such a key to simulate typing the characters missed on the keyboard layouts, e. g.:

            xdotool type 🫠

        Upper bound of the range can't be exceeded because X key codes are represented by 8-bit
        unsigned integer. Lower bound looks soft, but key codes in range [1…7] can't be converted
        to Linux kernel key codes anyway.
    **/
    struct key_t: t::key_t< KeyCode, 8 > {

        using parent_t = myself_t;
        using myself_t = key_t;

        /**
            X Window System key codes are biased Linux key codes:

            @code
                x_key_code = linux_key_code + offset;
            @endcode

            Offset is not documented but hardcoded in `xorg-x11-drv-libinput` sources, see
            `XORG_KEYCODE_OFFSET` macro in `xf86libinput.c`.
        **/
        static rep_t constexpr offset = min;

        /** Constructs key. If the given code is not valid, `val_error_t` exception is thrown. **/
        explicit key_t( rep_t _code = none );

        /**
            Constructs `x::key_t` from `linux::key_t`. If the given Linux kernel key can't be
            converted to X key, `val_error_t` exception is thrown.
        **/
        explicit key_t( linux::key_t key );

        /** Converts X Window System key to Linux kernel key. **/
        linux::key_t linux() const;

    };

    // ---------------------------------------------------------------------------------------------
    // btn_t
    // ---------------------------------------------------------------------------------------------

    /**
        X Window System button code. X uses 8-bit integer to represent mouse button code, so the
        highest possible button code is 255. However, I have to map X button codes to Linux kernel
        key codes. Linux kernel key code space has 16 slots dedicated for mouse buttons (starting
        from `BTN_MOUSE` and up to but not including `BTN_JOYSTICK`). However, X button codes 4, 5,
        6, and 7 are to report mouse wheel scroll events and are not mapped to Linux key codes.
        Thus, I have 20 X button codes, 16 of them are mapped to Linux keyt codes and 4 are
        ignored.
    **/
    struct btn_t: t::key_t< KeyCode, 1, 20 > {

        using parent_t = myself_t;
        using myself_t = btn_t;

        static rep_t constexpr left      =  1; ///< X code of the left mouse button.
        static rep_t constexpr middle    =  2; ///< X code of the middle mouse button.
        static rep_t constexpr right     =  3; ///< X code of the right mouse button.
        static rep_t constexpr wheel_min =  4;
            ///< X code of the first pseudo button used to report mouse wheel scroll.
        static rep_t constexpr wheel_max =  7;
            ///< X code of the last pseudo button used to report mouse wheel scroll.
        static rep_t constexpr fourth    =  8; ///< X code of the fourth mouse button.

        /**
            Constructs button. If the given code is not valid, `val_error_t` exception is thrown.
        **/
        explicit btn_t( rep_t code = none );

        /**
            Constructs `x::btn_t` from `linux::key_t`. Throws `val_error_t` exception if Linux
            kernel key code can't be converted to X Window System button code.

            Function `btn_xorg2linux` from `x86libinput.c` implements the following mapping from
            Linux key codes to X mouse buttons:

                Linux key code → X button code
                ---------------------------------------
                0              → 0
                BTN_LEFT       → 1
                BTN_MIDDLE     → 2
                BTN_RIGHT      → 3
                …              → 8 + lkey - BTN_SIDE
                ---------------------------------------

            Note: X buttons 4…7 are used to report mouse wheel rotation (4 and 5 used by vertical
            wheel, 6 and 7 used by horizontal wheel). `BTN_SIDE` (the fourth mouse button),
            `BTN_EXTRA` (the fifth)  and others are mapped to X butons 8, 9, etc.
        **/
        explicit btn_t( linux::key_t key );

        /**
            Converts X Window System button to Linux kernel key code. For pseudo buttons used for
            report mouse wheel rotaion, result will be `linux::key_t::none`.
        **/
        linux::key_t linux() const;

    };

    /**
        Initializes the X Window System library. Call it in the beginning, before creating the
        first thread.
    **/
    void init();

    // ---------------------------------------------------------------------------------------------
    // error_t
    // ---------------------------------------------------------------------------------------------

    /** X Window System exception class. **/
    class error_t: public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
    }; // class error_t

    // ---------------------------------------------------------------------------------------------
    // display_t
    // ---------------------------------------------------------------------------------------------

    /**
        `Display *` wrapper. Calls `XOpenDisplay` in constructor, and makes sure `XCloseDisplay` is
        called in destructor.
    **/
    class display_t: public object_t {

        public:

            explicit display_t();
            virtual ~display_t();

            operator Display *() const;

            int connection() const;

            void flush();
            void sync( bool discard = false );

        private:

            Display * _rep = nullptr;

    }; // class display_t

    using display_p = ptr_t< display_t >;

    // ---------------------------------------------------------------------------------------------
    // atom_t
    // ---------------------------------------------------------------------------------------------

    class atom_t: public object_t {

        public:

            explicit atom_t( display_t const & display, Atom atom );

            string_t name();

        private:

            display_t const &   _display;
            Atom                _rep;

    }; // atom_t

    // =============================================================================================
    // XRecord
    // =============================================================================================

    /// XRecord wrappers
    namespace record {

        // -----------------------------------------------------------------------------------------
        // intercept_data_t
        // -----------------------------------------------------------------------------------------

        /**
            `XRecordInterceptData *` wrapper. Makes sure `XRecordFreeData` is called.
        **/
        class intercept_data_t: public object_t {

            public:

                explicit intercept_data_t( XRecordInterceptData * data = nullptr );
                virtual ~intercept_data_t();

            public:

                XRecordInterceptData * data;

        }; // class intercept_data_t

        // -----------------------------------------------------------------------------------------
        // range_t
        // -----------------------------------------------------------------------------------------

        class range_t: public object_t {

            public:

                explicit range_t( unsigned char first, unsigned char last );
                virtual ~range_t();

            public:

                XRecordRange * _rep = nullptr;

        }; // class range_t

        // -----------------------------------------------------------------------------------------
        // context_t
        // -----------------------------------------------------------------------------------------

        class context_t: public object_t {

            public:

                using on_intercept_t = std::function< void( XRecordInterceptData const * ) >;

                explicit context_t(
                    on_intercept_t    on_intercept,
                    int               datum_flags = 0,
                    XRecordClientSpec client      = XRecordAllClients
                );
                virtual ~context_t();

                display_t & display();

                void enable();
                void disable();

            private:

                static void interceptor( XPointer closure, XRecordInterceptData * data );

                class thread_t: public posix::thread_t {
                    using parent_t = posix::thread_t;
                    friend class context_t;
                    public:
                        explicit thread_t( display_t & display );
                    protected:
                        virtual void body() override;
                    private:
                        display_t & _display;
                }; // thread_t

            private:

                enum state_t {
                    inited,
                    enabled,
                    disabled
                }; // enum state_t

                on_intercept_t  _on_intercept;
                state_t         _state  { inited };
                XRecordContext  _handle { None };
                display_t       _ctrl;
                display_t       _data;
                thread_t        _thread;

        }; // class context_t

    }; // namespace record

    /// XTest wrapper
    class test_t: public object_t {

        public:

            explicit test_t( display_t & display );

            void fake_key_event( unsigned int key, bool is_press, unsigned long delay = 0 );
            void fake_button_event( unsigned int button, bool is_press, unsigned long delay = 0 );

        private:

            display_t & _display;

    }; // class test_t

    // ---------------------------------------------------------------------------------------------
    // kb_t
    // ---------------------------------------------------------------------------------------------

    class kb_t: public object_t {

        public:

            struct autorepeat_rate_t {
                uint_t  timeout  = 0;
                uint_t  interval = 0;
            }; // struct autorepeat_rate_t

            /** Keyboard description. **/
            class desc_t: public object_t {

                friend class kb_t;

                private:

                    explicit desc_t( kb_t & kb, uint_t which );

                public:

                    virtual ~desc_t();

                    key_t::range_t key_range()   const;    ///< This is X key range, not Tapper!
                    strings_t      key_names()   const;
                    strings_t      group_names() const;

                private:

                    kb_t &      _kb;
                    XkbDescPtr  _rep = nullptr;

            }; // class desc_t

            using desc_p = ptr_t< desc_t >;

        public:

            explicit kb_t( display_t & display );

            display_t &       display();
            autorepeat_rate_t autorepeat_rate();
            void              lock_group( uint_t group );
            void              bell();
            desc_p            desc( uint_t which = XkbAllComponentsMask );

        private:

            display_t &     _display;

    }; // class kb_t

    using kb_p = ptr_t< kb_t >;

}; // namespace x
}; // namespace tapper

#endif // _TAPPER_X_HPP_

// end of file //
