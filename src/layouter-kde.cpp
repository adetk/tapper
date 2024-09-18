/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter-kde.cpp

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
    `layouter::kde_t` class implementation.

    @sa layouter-kde.h
    @sa layouter-kde.hpp
**/

#include "layouter-kde.hpp"
#include "layouter-kde.h"

#include <glibmm/fileutils.h>   // FileError
#include <glibmm/keyfile.h>

#include <giomm/settings.h>

#include "string.hpp"
#include "xdg.hpp"

tapper::layouter_t *
layouter_kde_create(
) {
    THIS( nullptr );
    DBG( "Creating kde layouter…" );
    return new tapper::layouter::kde_t;
};

namespace tapper {
namespace layouter {

// -------------------------------------------------------------------------------------------------
// kde_t
// -------------------------------------------------------------------------------------------------

kde_t::kde_t(
):
    OBJECT_T(),
    _dbus(
        "org.kde.keyboard",
        "/Layouts",
        "org.kde.KeyboardLayouts"
    )
{
}; // ctor

/** Returns `"KDE"`. **/
string_t
kde_t::type(
) {
    return "KDE";
}; // type

/**
    Activates specified keyboard layout.
**/
void
kde_t::activate(
    layout_t layout
) {
    assert( layout.index );
    auto reply = _dbus.call(
        "setLayout",
        Glib::Variant< std::tuple< guint > >::create( std::make_tuple( layout.index - 1 ) ),
        "(b)",
        std::chrono::milliseconds( 10 )
    );
    auto ok = CAST_DYNAMIC( Variant< bool >, reply.get_child( 0 ) ).get();
    if ( not ok ) {
        WRN( "Can't activate layout " << layout << ": No such layout." );
        return;
    };
    if ( bell() ) {
        /*
            TODO: Try to play sound with GTK libraries?
        */
    };
}; // activate

/** Extracts repeat delay from KDE settings and returns it. **/
time_t
kde_t::_get_repeat_delay(
) {
    string_t const file  = "kcminputrc";
    string_t const group = "Keyboard";
    string_t const key   = "RepeatDelay";
    string_t const name  = group + "/" + key;
    for ( auto const & config_dir: xdg::config_dirs() ) {
        auto const path = config_dir + "/" + file;
        Glib::KeyFile settings;
        bool loaded = false;
        try {
            loaded = settings.load_from_file( path );
        } catch ( Glib::FileError const & ex ) {
            if ( ex.matches( G_FILE_ERROR, G_FILE_ERROR_NOENT ) ) {
                INF( "Can't load file " << q( path ) << ": " << ex.what() );
            } else {
                WRN( "Can't load file " << q( path ) << ": " << ex.what() );
            };
        };
        if ( not loaded ) {
            continue;
        };
        if ( not settings.has_group( group ) ) {
            INF( "Group " << q( group ) << " not found in file " << q( path ) << "." );
            continue;
        };
        if ( not settings.has_key( group, key ) ) {
            INF(
                "Key " << q( key ) << " not found in group " << q( group )
                    << " in file " << q( path ) << "."
            );
            continue;
        };
        auto value = settings.get_value( group, key );
        if ( not like_cardinal( value ) ) {
            WRN(
                "Bad file " << q( path ) << ": "
                    << "Bad " << q( name ) << " value " << q( value ) << ": "
                    << " The value does not look like a cardinal number; ignore it."
            );
            continue;
        };
        auto delay = val< time_t >( value );
        if ( delay == 0 ) {
            WRN(
                "Bad file " << q( path ) << ": "
                    << "Bad " << q( name ) << " value " << q( value ) << ": "
                    << " The value must be greater than zero; ignore it."
            );
            continue;
        };
        return delay;
    };
    INF( "No keyboard repeat delay found in KDE configuration files." );
    return 0;
};

strings_t
kde_t::_get_layout_names(
) {
    strings_t layouts;
    auto reply = _dbus.call(
        "getLayoutsList",
        Glib::VariantContainerBase(),
        "(a(sss))",
        std::chrono::milliseconds( 100 )
    );
    auto array = CAST_DYNAMIC( VariantContainerBase, reply.get_child( 0 ) );
    for ( size_t i = 0, end = array.get_n_children(); i < end; ++ i ) {
        auto item       = CAST_DYNAMIC( VariantContainerBase, array.get_child( i ) );
        //~ auto lang       = CAST_DYNAMIC( Variant< string_t >, item.get_child( 0 ) ).get();
        //~ auto ???        = CAST_DYNAMIC( Variant< string_t >, item.get_child( 1 ) ).get();
        auto long_name  = CAST_DYNAMIC( Variant< string_t >, item.get_child( 2 ) ).get();
        layouts.push_back( long_name );
    };
    return layouts;
};

}; // namespace layouter
}; // namespace tapper

// end of file //
