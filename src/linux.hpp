/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/linux.hpp

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
    Linux wrappers interfase.

    This code was originally a part of listener-libinput module. However, (1) it is Linux-specific
    code, there is nothing about libinput; (2) it is much easier to write trivial unit tests for
    standalone functions rather than class methods.

    @sa linux.cpp
**/

#ifndef _TAPPER_LINUX_HPP_
#define _TAPPER_LINUX_HPP_

#include "base.hpp"

#include <set>

#include "key.hpp"

namespace tapper {
/**
    Linux wrappers.

    Of course there are no wrappers for entire Linux kernel. Only few Linux-functions used by
    Tapper are wrapped here.
**/
namespace linux {

/*
    Let's define few constants borrowed from <linux/input-event-codes.h> so other parts of Tapper
    does not depend on that header. (Tapper can be built without <linux/input-event-codes.h>.)
*/

enum event_type_t {
    ev_syn = 0,
    ev_key = 1,
};

enum syn_event_t {
    syn_report = 0,
};

/**
    Linux kernel key code (== user input event code) . Currently Linux kernel key codes are in
    range [1…0x2FF]. Key code 0 is reserved and seems unused. However, X Window System may use X
    key code 8 (e. g. `xdotool type 🫠`) which matches Linux key code 0, so let's include 0 into the
    valid key range. Also, do not reject key codes out of this range because such codes may appear
    in future versions of the Linux kernel.
**/
struct key_t: t::key_t< uint_t, 0, 0x2FF > {

    using parent_t = myself_t;
    using myself_t = key_t;

    static rep_t constexpr btn_min    = 0x110;        ///< The smallest code of all mouse buttons.
    static rep_t constexpr btn_max    = btn_min + 15; ///< The largest code of all mouse buttons.
    static rep_t constexpr btn_left   = btn_min +  0; ///< The code of the left mouse button.
    static rep_t constexpr btn_right  = btn_min +  1; ///< The code of the right mouse button.
    static rep_t constexpr btn_middle = btn_min +  2; ///< The code of the middle mouse button.
    static rep_t constexpr btn_fourth = btn_min +  3; ///< The code of the fourth mouse button.

    using parent_t::parent_t;

    /** Returns `true` if key is a mouse button. **/
    bool is_mouse_button() const {
        return btn_min <= _code and _code <= btn_max;
    };

};

/** Set of keys. **/
using keys_t = std::set< key_t >;
using keys_p = ptr_t< keys_t >;

/**
    Returns range of key codes. `key_t::none` is not included to the range; the range includes keys
    from `1` upto and including `key_t::max`.
**/
key_t::range_t key_range();

/** Returns set of keys. Only keys with names are included into the set. **/
keys_t keys();

/** Returns key by the given key name. **/
key_t key( string_t const & name );

/** Returns the primary name of the given key. **/
string_t key_name( key_t key );

/**
    Returns all names (including aliases) of the given key. Returns empty vector if the given key
    does not have any names.
**/
strings_t key_names( key_t key );

}; // namespace linux
}; // namespace tapper

#endif // _TAPPER_LINUX_HPP_

// end of file //
