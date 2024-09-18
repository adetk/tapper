/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter-gnome.cpp

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
    `layouter::gnome_t` class implementation.

    @sa layouter-gnome.h
    @sa layouter-gnome.hpp
**/

#include "layouter-gnome.hpp"
#include "layouter-gnome.h"

#include <giomm/dbuswatchname.h>
#include <giomm/settings.h>

#include "string.hpp"

tapper::layouter_t *
layouter_gnome_create(
) {
    THIS( nullptr );
    DBG( "Creating gnome layouter…" );
    return new tapper::layouter::gnome_t;
};

namespace tapper {
namespace layouter {

using connection_t = Glib::RefPtr< Gio::DBus::Connection >;

// -------------------------------------------------------------------------------------------------
// gnome_t
// -------------------------------------------------------------------------------------------------

gnome_t::gnome_t(
):
    OBJECT_T(),
    _dbus(
        "org.gnome.Shell",
        "/org/gnome/Shell/Extensions/Agism",
        "io.sourceforge.Agism"
    )
{
}; // ctor

/** Returns `"GNOME"`. **/
string_t
gnome_t::type(
) {
    return "GNOME";
}; // type

/** Extracts autorepeat delay from GNOME settings and returns it. **/
time_t
gnome_t::_get_repeat_delay(
) {
    auto settings = Gio::Settings::create( "org.gnome.desktop.peripherals.keyboard" );
    return settings->get_uint( "delay" );
};

/**
    Activates specified keyboard layout.
**/
void
gnome_t::activate(
    layout_t layout
) {
    TRACE();
    assert( layout.index );
    /*
        If GNOME Shell restarts, activating a layout will fail with one of the errors:

            1.  GDBus.Error:org.freedesktop.DBus.Error.UnknownMethod: Object does not exist at
                path “/org/gnome/Shell/Extensions/Agism”
            2.  dbus::caller_error_t: There is no ‘org.gnome.Shell’ session bus.

        Let's print a warning and continue, because it is not a fatal error: when GNOME Shell
        finishes restarting, everything will work.

        // TODO: Can GNOME Shell restart in Wayland session?
    */
    try {
        auto reply = _dbus.call(
            "ActivateInputSource",
            Glib::Variant< std::tuple< guint, bool, string_t > >::create(
                std::make_tuple( layout.index  - 1, false, bell() ? "bell" : "" )
            ),
            "(b)",
            std::chrono::milliseconds( 10 )
        );
        auto ok = CAST_DYNAMIC( Variant< bool >, reply.get_child( 0 ) ).get();
        if ( not ok ) {
            WRN( "Can't activate layout " << layout << ": No such layout." );
        };
    } catch ( dbus_t::error_t const & ex ) {
        if ( ex.code() == dbus_t::error_t::code_t::no_such_bus ) {
            WRN( "Can't activate layout " << layout << ": " << ex.what() );
        } else {
            throw;
        };
    } catch ( Glib::Error const & ex ) {
        if ( ex.matches( G_DBUS_ERROR, G_DBUS_ERROR_UNKNOWN_METHOD ) ) {
            WRN( "Can't activate layout " << layout << ": " << ex.what() );
        } else {
            throw;
        };
    };
}; // activate

strings_t
gnome_t::_get_layout_names(
) {
    strings_t layouts;
    auto reply = _dbus.call(
        "GetInputSources",
        Glib::VariantContainerBase(),
        "(a(ussss))",
        std::chrono::milliseconds( 100 )
    );
    auto array = CAST_DYNAMIC( VariantContainerBase, reply.get_child( 0 ) );
    for ( size_t i = 0, end = array.get_n_children(); i < end; ++ i ) {
        auto item       = CAST_DYNAMIC( VariantContainerBase, array.get_child( i ) );
        //~ auto index      = CAST_DYNAMIC( Variant< guint >,    item.get_child( 0 ) ).get();
        //~ auto id         = CAST_DYNAMIC( Variant< string_t >, item.get_child( 1 ) ).get();
        //~ auto short_name = CAST_DYNAMIC( Variant< string_t >, item.get_child( 2 ) ).get();
        auto long_name  = CAST_DYNAMIC( Variant< string_t >, item.get_child( 3 ) ).get();
        //~ auto type       = CAST_DYNAMIC( Variant< string_t >, item.get_child( 4 ) ).get();
        layouts.push_back( long_name );
    };
    return layouts;
};

bool
gnome_t::_can_ring(
) {
    return true;
};

void
gnome_t::_start(
) {
    _id = Gio::DBus::watch_name(
        // Gio::DBus::BUS_TYPE_SESSION,
        Gio::DBus::BusType::SESSION,
        "org.gnome.SessionManager",
        [ this ](
            connection_t const &    connection,
            string_t                name,
            string_t const &
        ) {
            DBG( "Name " << q( name ) << " appeared in the session bus." );
            _sub = connection->signal_subscribe(
                [ this ](
                    connection_t                       conn,
                    string_t const &                   sender,
                    string_t const &                   object,
                    string_t const &                   iface,
                    string_t const &                   signal,
                    Glib::VariantContainerBase const & params
                ) {
                    DBG(
                        "D-Bus signal: "
                            << "sender=" << sender << ", "
                            << "object=" << object << ", "
                            << "iface="  << iface  << ", "
                            << "signal=" << signal << "."
                    );
                    if (
                        object != "/org/gnome/SessionManager"
                        or iface != "org.freedesktop.DBus.Properties"
                        or signal != "PropertiesChanged"
                    ) {
                        return;         // It is not a signal I am looking for.
                    };
                    string_t type = params.get_type_string();
                    if ( type != "(sa{sv}as)" ) {
                        ERR(
                            "Unexpected signal parameters."
                        ); // TODO: signal, exp. and act. types.
                    };
                    // `get_child()` is not a constant method.
                    // Let's make a copy for easier access.
                    Glib::VariantContainerBase p = params;
                    auto _iface = CAST_DYNAMIC( Variant< string_t >, p.get_child( 0 ) ).get();
                    if ( _iface != "org.gnome.SessionManager" ) {
                        WRN( "Unexpected:" ); // TODO
                        return;
                    };
                    using hash_t = std::map< string_t, Glib::VariantBase >;
                    auto hash = CAST_DYNAMIC( Variant< hash_t >, p.get_child( 1 ) ).get();
                    auto it = hash.find( "SessionIsActive" );
                    if ( it == hash.end() ) {   // `SessionIsActive` property is not changed.
                        return;
                    };
                    auto value = CAST_DYNAMIC( Variant< bool >, it->second ).get();
                    DBG( "Session is " << ( value ? "" : "not " ) << "active." );
                    if ( _handler ) {
                        _handler( value );
                    };
                },
                "",                                 // sender
                "org.freedesktop.DBus.Properties",  // interface
                "PropertiesChanged",                // signal
                "/org/gnome/SessionManager"         // object
            );
        },
        [ this ](
            connection_t const &    connection,
            string_t                name
        ) {
            DBG( "Name " << q( name ) << " vanished in the session bus." );
            connection->signal_unsubscribe( _sub );
        }
    );
};

void
gnome_t::_stop(
) {
    Gio::DBus::unwatch_name( _id );
};

}; // namespace layouter
}; // namespace tapper

// end of file //
