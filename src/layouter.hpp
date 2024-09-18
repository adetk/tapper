/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter.hpp

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
    `layouter_t` abstract class interface.

    @sa layouter.cpp
**/

#ifndef _TAPPER_LAYOUTER_HPP_
#define _TAPPER_LAYOUTER_HPP_

#include "base.hpp"

#include <functional>

#include "settings.hpp"
#include "types.hpp"

namespace tapper {

// -------------------------------------------------------------------------------------------------
// layouter_t
// -------------------------------------------------------------------------------------------------

class layouter_t;
using layouter_p = ptr_t< layouter_t >;

/**
    Layouter abstract interface and factory.

    "Layouter" is a shorter name of "Layout manager". The primary goal of a layouter — providing a
    list of available layouts (`layouts()`) and activating a layout (`activate()`).

    "Available layout" means that layout is immediatelly available to use and can be activated. The
    system may have hundreds of keyboard layouts, but usually only few of them are immediatelly
    available. In order to use a layout, user should "configure" it by using an application named
    "Control Center", "Setting", or similar.

    Note that X Window System allows to load not more than 4 layouts, and most of desktops respect
    the limit. Some desktops ignore the limit and allow to configure more than 4 layouts, but only
    4 of them can be actually used. GNOME is an exeption — it allows to configure more than 4
    layouts, even if it works on top of X Window System. GNOME Shell dynamically reloads layouts to
    X Window System to lift the limit. I am not aware if similar limit exits in Wayland.

    ALso a layouter provides few utilities — computing a layout name by given layout index, and
    vice versa.

    A layouter may notify about user changes in user session state — if the session is active or
    not. Session is not active if user switched to login manager or to another session. Currently
    only GNOME layouter can do it. It may not be the best possible design desision, but GNOME
    layouter is the component closest to GNOME. It already communicates with GNOME through D-Bus,
    so let it listen the bus for one more signal.
**/
class layouter_t {

    public:         // types

        /** Layouter exceptions. **/
        class error_t: public std::runtime_error {
            public:
                using std::runtime_error::runtime_error;
        }; // class error_t

        using on_event_t = std::function< void( bool ) >;

    public:         // methods

        /** Layouter factory. Returns pointer to a new layouter of the specified type. **/
        static layouter_t * create( settings_t::layouter_t type );

        virtual ~layouter_t() = default;

        /**
            Any descendant should override this method. The method should return layouter type
            name, e. g. "GNOME", "KDE" or "Xkb".
        **/
        virtual string_t type() = 0;

        /**
            Returns keyboard delay in milliseconds.
        **/
        time_t repeat_delay();

        /**
            Returns the range of valid layout indices for the layouter. Range represents
            "theoretical" lower and upper bounds, not all indices in the range may represent
            actually configured and ready-to-use layouts.
        **/
        virtual layout_range_t layout_range();

        /**
            Returns the set of layout indices for the layouter. In contrast to `layout_range`, all
            the indices in the set represents actully available layouts at some moment (e. g. at
            the moment of layoter initialization).
        **/
        layouts_t layouts();

        /**
            Returns the name of layout for given layout index.
        **/
        string_t layout_name( layout_t layout );

        /**
            Returns the full name of layout for given layout index. Full layout name consists of
            layout index, colon, and layout name (colon is not added if layout name is empty).
        **/
        string_t layout_full_name( layout_t layout );

        /**
            Returns the index of layout for given layout name.
        **/
        layout_t layout( string_t const & name );

        /**
            The method activates the given layout and ring the bell (if `_bell_` is `true` and
            layouter is capable to do it).

            Descentands are expected to override the method.
        **/
        virtual void activate(
            layout_t layout     ///< Index of the layout to activate.
        ) = 0;

        /**
            Start layouter.

            @param bell — If `true`, the bell is enabled, and disabled otherwise.

            @param handler — This function, if specified, will be called when user session changes
            its status from active to inactive and back.

            Note that `start()` is actually starts the user session status monitoring. Most of the
            methods can be called before `start()`.
        **/
        void start( bool bell = false, on_event_t handler = nullptr );

        /**
            Returns bell status: `true` is bell is enabled and `false` otherwise. It does not ring
            the bell.
        **/
        bool bell();

        void stop();

    protected:      // methods

        virtual void _start() {};
        virtual void _stop()  {};

        /**
            The returns keyboard repeat delay (amount of time the user has to hold a key down
            before the system will generate repeating key press events). Note that `repeat_delay`
            is just a front-end, the actual functionality is provided by this method.

            Non-dummy descendatns are expected to override the method.
         **/
        virtual time_t  _get_repeat_delay();

        /**
            The method returns list of layout names know to the layouter. To avoid repeated
            requests the result of the method is cached, see `_layout_names` and `_layout_names_`.

            Non-dummy descendatns are expected to override the method.
        **/
        virtual strings_t _get_layout_names();

        /**
            The method returns `true` if the layouter can ring the bell, and `false` otherwise.
            Default implementation returns `false`. If the decendant can ring, it should override
            the method.

            Non-dummy descendatns are expected to override the method.
        **/
        virtual bool _can_ring();

    private:        // methods

        strings_t const & _layout_names();

    protected:      // data

        on_event_t  _handler { nullptr };      ///< Function to call on every user input event.

    private:

        bool        _bell_ { false };
            ///< Bell status: bell is enabled (if `true`) or disabled (if `false`).
        time_t      _repeat_delay_ { 0 };
            ///< Cached repeat delay returned by `_get_repeat_delay`.
        strings_p   _layout_names_;
            ///< Cached layout names returned by `_get_layout_names`.

}; // class layouter_t

/// Namespace for concrete layouters.
namespace layouter {
}; // namespace layouter

}; // namespace tapper

#endif // _TAPPER_LAYOUTER_HPP_

// end of file //
