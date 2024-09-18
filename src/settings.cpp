/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/settings.cpp

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
    `settings_t` class implementation.
**/

#include "settings.hpp"

#include <stdexcept>

#include "string.hpp"

#if WITH_GLIB
    #include <giomm/settings.h>
#endif // WITH_GLIB

namespace tapper {

/** Construct reverse map type. **/
template< typename map_t >
using reverse_map_t = std::map< typename map_t::mapped_type, typename map_t::key_type >;

/**
    Reverse a map, exclude from result negative keys.
**/
template< typename map_t >
static
reverse_map_t< map_t >
reverse(
    map_t const & map
) {
    reverse_map_t< map_t > reversed_map;
    for ( auto & it: map ) {
        if ( int( it.first ) >= 0 ) {
            // Let's exclude negative items from result, so "unset" items will not appear there.
            reversed_map.insert( { it.second, it.first } );
        };
    };
    return reversed_map;
};

/** Returns a vector containing all map keys. **/
template< typename map_t >
static
std::vector< typename map_t::key_type >
map_keys(
    map_t const & map
) {
    std::vector< typename map_t::key_type > keys;
    std::transform(
        map.begin(),
        map.end(),
        std::back_inserter( keys ),
        []( typename map_t::value_type const & it ) {
            return it.first;
        }
    );
    return keys;
};

/**
    The code below uses macro `ERR`, which throws an exception of type `error_t`.
**/
using error_t = std::domain_error;

using bell_to_str_t     = std::map< settings_t::bell_t,     string_t >;
using listener_to_str_t = std::map< settings_t::listener_t, string_t >;
using layouter_to_str_t = std::map< settings_t::layouter_t, string_t >;
using emitter_to_str_t  = std::map< settings_t::emitter_t,  string_t >;

using str_to_bell_t     = reverse_map_t< bell_to_str_t     >;
using str_to_listener_t = reverse_map_t< listener_to_str_t >;
using str_to_layouter_t = reverse_map_t< layouter_to_str_t >;
using str_to_emitter_t  = reverse_map_t< emitter_to_str_t  >;

static bell_to_str_t const bell_to_str {
    { settings_t::bell_t::unset,        "unset",    },
    { settings_t::bell_t::disabled,     "disabled", },
    { settings_t::bell_t::enabled,      "enabled",  },
};

static listener_to_str_t const listener_to_str {
    { settings_t::listener_t::unset,    "unset",    },
    { settings_t::listener_t::Auto,     "auto",     },
    { settings_t::listener_t::libinput, "libinput", },
    { settings_t::listener_t::xrecord,  "xrecord",  },
};

static layouter_to_str_t const layouter_to_str {
    { settings_t::layouter_t::unset,    "unset",    },
    { settings_t::layouter_t::Auto,     "auto",     },
    { settings_t::layouter_t::dummy,    "dummy",    },
    { settings_t::layouter_t::gnome,    "gnome",    },
    { settings_t::layouter_t::kde,      "kde",      },
    { settings_t::layouter_t::xkb,      "xkb",      },
};

static emitter_to_str_t const emitter_to_str {
    { settings_t::emitter_t::unset,     "unset",    },
    { settings_t::emitter_t::Auto,      "auto",     },
    { settings_t::emitter_t::dummy,     "dummy",    },
    { settings_t::emitter_t::libevdev,  "libevdev", },
    { settings_t::emitter_t::xtest,     "xtest",    },
};

static str_to_bell_t     const str_to_bell     = reverse( bell_to_str     );
static str_to_listener_t const str_to_listener = reverse( listener_to_str );
static str_to_layouter_t const str_to_layouter = reverse( layouter_to_str );
static str_to_emitter_t  const str_to_emitter  = reverse( emitter_to_str  );

/** Returns list of all listener types. **/
strings_t
settings_t::listeners(
) {
    return map_keys( str_to_listener );
};

/** Returns list of all layouter types. **/
strings_t
settings_t::layouters(
) {
    return map_keys( str_to_layouter );
};

/** Returns list of all emitter types. **/
strings_t
settings_t::emitters(
) {
    return map_keys( str_to_emitter );
};

void
settings_t::reset(
) {
    #if WITH_GLIB
        auto g_settings = Gio::Settings::create( PACKAGE_ID );
        g_settings->reset( "listener" );
        g_settings->reset( "layouter" );
        g_settings->reset( "emitter" );
        g_settings->reset( "bell" );
        g_settings->reset( "assignments" );
        // Make sure changes are really applied:
        g_settings_sync(); // glibmm does not have such a function, let's call gio directly.
    #else
        ERR( "Program is built without glib." );
    #endif // WITH_GLIB
};

#if WITH_GLIB

    /*
        `Gio::Settings` does not know how to handle `assignments_t` (because of using enums
        `key_t`, and `action_t`), so I have to convert it to similar structure consisting of simple
        integers. I am not sure it is the best way, but for now it looks to be the simplest.
    */
    using g_key_t         = int_t;
    using g_actions_t     = string_t;
    using g_assignments_t = std::map< g_key_t, g_actions_t >;
    using g_settings_t    = Glib::RefPtr< Gio::Settings >;

    /**
        Loads enum value from GSettings. Checks that loaded value in range `[0…max_value]`, abd
        throws `val_error_t` exception if value is out of the range.
    **/
    static
    int_t
    _load_enum(
        g_settings_t &      settings,
        string_t const &    name,           ///< Key name.
        int                 max_value       ///< Max allowed value.
    ) {
        using error_t = val_error_t;
        try {
            using range_t = t::range_t< int_t >;
            int_t result = settings->get_enum( name );
            range_t( 0, max_value ).check( result );
            return result;
        } catch ( error_t const & ex ) {
            ERR( "Bad " << name << ": " << ex.what() );
        };
    };

    /**
        Loads enum value from GSettings. `_load_enum()` returns `int_t` value, while this function
        returns enumeration, not integer value (and does not require max allowed value).
    **/
    template< typename result_t >
    static
    result_t
    load_enum(
        g_settings_t &      settings,
        string_t const &    name            ///< Key name.
    ) {
        return result_t( _load_enum( settings, name, int_t( result_t::max ) ) );
    };

    /** Loads assignments from GSettings. **/
    static
    assignments_t
    load_assignments(
        g_settings_t &      settings,
        string_t const &    name            ///< Key name.
    ) {
        using error_t = val_error_t;
        assignments_t result;
        try {
            Glib::Variant< g_assignments_t > variant;
            settings->get_value( "assignments", variant );
            for ( auto const & g_assignment: variant.get() ) {
                auto key = key_t( g_assignment.first );
                try {
                    key_t::range_t().check( key );
                } catch ( error_t const & ex ) {
                    ERR( "Bad key " << q( str( key ) ) << ": " << ex.what() );
                };
                result[ key ] = val< actions_t >( g_assignment.second );
            };
        } catch ( error_t const & ex ) {
            ERR( "Bad " << name << ": " << ex.what() );
        };
        return result;
    };

    /**
        Saves enum value to GSettings. If value less than zero, resets GSettings key to default
        value. (By convention, all `settings_t` enums has `unset` constant with value `-1`. Saving
        such `unset` value to GSettings resets it.)
    **/
    void
    save_enum(
        g_settings_t &          settings,
        string_t const &        name,       ///< Key name.
        int                     value       ///< Value to save.
    ) {
        if ( value < 0 ) {
            settings->reset( name );
        } else {
            settings->set_enum( name, value );
        };
    };

    /** Saves assignments to GSettings. **/
    void
    save_assignments(
        g_settings_t &          settings,
        string_t const &        name,       ///< Key name.
        assignments_t const &   assignments ///< Assignments to save.
    ) {
        g_assignments_t g_assignments;
        for ( auto const & assignment: assignments ) {
            g_assignments[ assignment.first.code() ] = str( assignment.second );
        };
        settings->set_value( name, Glib::Variant< g_assignments_t >::create( g_assignments ) );
    };

#endif // WITH_GLIB

/** Load settings from GSettings database. **/
settings_t
settings_t::load(
) {
    #if WITH_GLIB
        settings_t r;
        using error_t = val_error_t;
        auto g_settings = Gio::Settings::create( PACKAGE_ID );
        try {
            r.listener    = load_enum< listener_t >( g_settings, "listener" );
            r.layouter    = load_enum< layouter_t >( g_settings, "layouter" );
            r.emitter     = load_enum< emitter_t  >( g_settings, "emitter"  );
            r.bell        = load_enum< bell_t     >( g_settings, "bell"     );
            r.assignments = load_assignments( g_settings, "assignments" );
            DBG( "Settings loaded: " << r );
        } catch ( error_t const & ex ) {
            ERR( "Bad settings: " << ex.what() );
        };
        return r;
    #else
        ERR( "Program is built without glib." );
    #endif // WITH_GLIB
};

/** Save settings to GSettings database. **/
void
settings_t::save(
) const {
    #if WITH_GLIB
        auto g_settings = Gio::Settings::create( PACKAGE_ID );
        // Options:
        save_enum( g_settings, "listener", int( listener ) );
        save_enum( g_settings, "layouter", int( layouter ) );
        save_enum( g_settings, "emitter",  int( emitter  ) );
        save_enum( g_settings, "bell",     int( bell     ) );
        // Assignments:
        save_assignments( g_settings, "assignments", assignments );
        // Make sure changes are really applied:
        g_settings_sync(); // glibmm does not have such a function, let's call gio directly.
        DBG( "Settings saved: " << * this );
    #else
        ERR( "Program is built without glib." );
    #endif // WITH_GLIB
};

string_t
settings_t::str(
) const {
    return STR(
        "{ " <<
            "listener: "    << listener    << ", " <<
            "layouter: "    << layouter    << ", " <<
            "emitter: "     << emitter     << ", " <<
            "bell: "        << bell        << ", " <<
            "assignments: " << assignments <<
        " }"
    );
};

string_t
str(
    settings_t::bell_t      bell
) {
    return bell_to_str.at( bell );
};

string_t
str(
    settings_t::listener_t  listener
) {
    return listener_to_str.at( listener );
};

string_t
str(
    settings_t::layouter_t  layouter
) {
    return layouter_to_str.at( layouter );
};

string_t
str(
    settings_t::emitter_t   emitter
) {
    return emitter_to_str.at( emitter );
};

template<>
settings_t::listener_t
val< settings_t::listener_t >(
    string_t const & str
) {
    try {
        return str_to_listener.at( str );
    } catch ( std::out_of_range const & ) {
        ERR( "Bad listener " << q( str ) << "." );
    };
};

template<>
settings_t::layouter_t
val< settings_t::layouter_t >(
    string_t const & str
) {
    try {
        return str_to_layouter.at( str );
    } catch ( std::out_of_range const & ) {
        ERR( "Bad layouter " << q( str ) << "." );
    };
};

template<>
settings_t::emitter_t
val< settings_t::emitter_t >(
    string_t const & str
) {
    try {
        return str_to_emitter.at( str );
    } catch ( std::out_of_range const & ) {
        ERR( "Bad emitter " << q( str ) << "." );
    };
};

/**
    Merge two settings. For example, one settings is loaded from GSettings, another one is built
    from the command line. Non-default options from the second settings overrides corresponding
    options in the first settings. Key assignments are merged too.
**/
settings_t &
operator +=(
    settings_t &        lhs,
    settings_t const &  rhs
) {
    if ( rhs.listener != settings_t::listener_t::unset ) {
        lhs.listener = rhs.listener;
    };
    if ( rhs.layouter != settings_t::layouter_t::unset ) {
        lhs.layouter = rhs.layouter;
    };
    if ( rhs.emitter != settings_t::emitter_t::unset ) {
        lhs.emitter = rhs.emitter;
    };
    if ( rhs.bell != settings_t::bell_t::unset ) {
        lhs.bell = rhs.bell;
    };
    lhs.assignments += rhs.assignments;
    return lhs;
};

/** Merge two settings. **/
settings_t
operator +(
    settings_t const & lhs,
    settings_t const & rhs
) {
    auto r = lhs;
    r += rhs;
    return r;
};

}; // namespace tapper

// end of file //
