/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/settings.hpp

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
    `settings_t` class interface.

    @sa settings.cpp
**/

#ifndef _TAPPER_SETTINGS_HPP_
#define _TAPPER_SETTINGS_HPP_

#include "base.hpp"

#include <map>

#include "types.hpp"

namespace tapper {

/**
    Settings, a subject to save to and load from GSettings database.

    Settings can be saved in GSettings (dconf) database and loaded back.
**/
struct settings_t {

    /** Bell status: disabled or enabled. **/
    enum class bell_t {
        unset = -1,     ///< Bell status is not set yet.
        disabled,       ///< Layout activation will *not* be accompanied by a bell.
        enabled,        ///< Layout activation will be accompanied by a bell.
        max = enabled,
    };

    /**
        Type of listener.
        The values must be aligned with the values in .gschema.xml file!
    **/
    enum class listener_t {
        unset = -1,     ///< Listener type is not set yet.
        Auto,           // `auto` is a C++ keyword.
        libinput,
        xrecord,
        max = xrecord,
    };

    /**
        Type of layouter.
        The values must be aligned with the values in .gschema.xml file!
    **/
    enum class layouter_t {
        unset = -1,     ///< Layouter type is not set yet.
        Auto,           // `auto` is a C++ keyword.
        dummy,
        gnome,
        kde,
        xkb,
        max = xkb,
    };

    enum class emitter_t {
        unset = -1,     ///< Layouter type is not set yet.
        Auto,           // `auto` is a C++ keyword.
        dummy,
        libevdev,
        xtest,
        max = xtest,
    };

    static strings_t  listeners();
    static strings_t  layouters();
    static strings_t  emitters();

    static void       reset();
    static settings_t load();
    void              save() const;
    string_t          str()  const;

    // Data fields:
    listener_t    listener { listener_t::unset };
    layouter_t    layouter { layouter_t::unset };
    emitter_t     emitter  { emitter_t::unset  };
    bell_t        bell     { bell_t::unset     };
    assignments_t assignments;

}; // struct settings_t

string_t str( settings_t::bell_t     bell     );
string_t str( settings_t::listener_t listener );
string_t str( settings_t::layouter_t layouter );
string_t str( settings_t::emitter_t  emitter  );

template<> settings_t::listener_t val< settings_t::listener_t >( string_t const & str );
template<> settings_t::layouter_t val< settings_t::layouter_t >( string_t const & str );
template<> settings_t::emitter_t  val< settings_t::emitter_t  >( string_t const & str );

settings_t & operator +=( settings_t & lhs, settings_t const & rhs );
settings_t   operator +( settings_t const & lhs, settings_t const & rhs );

}; // namespace tapper

#endif // _TAPPER_SETTINGS_HPP_

// end of file //
