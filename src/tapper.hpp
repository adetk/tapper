/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/tapper.hpp

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
    `tapper_t` class interface.

    @sa tapper.cpp
**/

#ifndef _TAPPER_TAPPER_HPP_
#define _TAPPER_TAPPER_HPP_

#include "base.hpp"

#include <atomic>

#include "emitter.hpp"
#include "layouter.hpp"
#include "listener.hpp"
#include "settings.hpp"
#include "types.hpp"

namespace tapper {

// -------------------------------------------------------------------------------------------------
// tapper_t
// -------------------------------------------------------------------------------------------------

/**
    A component that orchestrates listener, layouter and emitter to do the work.

    The tapper looks at keyboard events (provided by a listener) detects taps, if an assigned key
    is tapped, tapper executes corresponding series of actions — activates keyboard layouts (with
    help from a layouter) and/or emulates keystrokes (with help from an emitter).

    A layouter may notify the tapper when the user session becomes active or inactive (currently
    only the GNOME layouter can do this). If the user session is inactive, the tapper continues to
    detect taps, but will not execute corresponding actions as not to affect another user session.
    Detecting the inactive user session is important only for the libinput listers, though, since
    the XRecord listener do not receive input events from another session anyway.

    Usage:

    @code
    listener_t listener( ... );
    layouter_t layouter( ... );
    emitter_t  emitter( ... );
    // Listener, layouter, and emitter should be created but not started.
    tapper_t   tapper( listener, layouter, emitter );
    tapper.start( ... );    // tapper starts emitter, layouter and listener.
    ...
    tapper.stop();
    @endcode
**/
class tapper_t: public object_t {

    public:         // methods

        /**
            Listener, layouter and emitter should not be started.
        **/
        explicit tapper_t(
            listener_t &    listener,
            layouter_t &    layouter,
            emitter_t &     emitter
        );

        ~tapper_t();

        /**
            Starts the tapper.

            @param bell — If `true`, layout activation will be accompanied by a sound ("the bell
            will ring"). If `false`, the bell will not ring. Note, that layoter is responsible for
            sound, not tapper. Some layouts can't ring the bell.

            @param show_taps — If `true`, tapper will print a message to standard output stream
            "Key *code*:*name* tapped.". It is useful to discover key codes and names. If `false`,
            tapper will not print such messages.
        **/
        void start( assignments_t const & assignments, bool bell = false, bool show_taps = false );

        /**
            Stops the tapper. Tapper, in turn, will stop listener, layouer and emitter.
        **/
        void stop();

    private:            // types

        using event_t = listener_t::event_t;

    private:            // methods

        void _on_event( event_t const & event );
        void _on_tap( key_t key );

    private:            // data

        listener_t &  _listener;
        layouter_t &  _layouter;
        emitter_t &   _emitter;
        assignments_t _assignments;
        bool          _show_taps { false };
        time_t        _repeat_delay { 0 };
            // TODO: Update it?

        key_t::range_t _key_range;

        /**
            Keyboard state. It maps keys to boolean values. `true` means the corresponding key is
            pressed now, `false` means the key is not pressed.
        **/
        std::vector< bool > _key_state;

        /**
            Number of currently pressed keys. The number of currently pressed keys can be achieved
            from `_key_state`, but scanning the `_key_state` is rather slow. Maintaining number of
            pressed keys is simple and fast.
        **/
        uint_t _pressed_keys { 0 };

        /**
            Key code of the last pressed key or 0. Used to detect taps: if code of released key
            equals to code of the last pressed key, it could be a tap.
        **/
        key_t _last_key;

        /**
            Time of the last key press event, which code was saved in `last_pressed_key`.
            Meaningful only if `last_pressed_key` is not 0.
        **/
        time_t _pressed_at { 0 };

        /**
            `true`, if tapper is active, and `false` otherwise. If tapper is not active, it
            continues to look at user input events (to maintain keyboard state; if events are
            provided by a listener), but does not try to execute commands (activate layouts and/or
            emit keystrokes).

            Note that `_active` is written and read by different threads.
        **/
        std::atomic< bool > _active { true };

}; // class tapper_t

}; // namespace tapper

#endif // _TAPPER_TAPPER_HPP_

// end of file //
