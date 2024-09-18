/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter.cpp

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
    `layouter_t` abstract class implementation.
**/

#include "layouter.hpp"

#include "layouter-dummy.h"
#if ENABLE_GNOME
    #include "layouter-gnome.h"
#endif // ENABLE_GNOME
#if ENABLE_KDE
    #include "layouter-kde.h"
#endif // ENABLE_KDE
#if WITH_X
    #include "layouter-xkb.h"
#endif // WITH_X

namespace tapper {

// -------------------------------------------------------------------------------------------------
// layouter_t
// -------------------------------------------------------------------------------------------------

layouter_t *
layouter_t::create(
    settings_t::layouter_t type
) {
    layouter_t * layouter = nullptr;
    switch ( type ) {
        case settings_t::layouter_t::unset: {
            assert( 0 );
        } break;
        case settings_t::layouter_t::Auto: {
            assert( 0 );
        } break;
        case settings_t::layouter_t::dummy: {
                layouter = layouter_dummy_create();
        } break;
        case settings_t::layouter_t::gnome: {
            #if ENABLE_GNOME
                layouter = layouter_gnome_create();
            #endif // ENABLE_GNOME
        } break;
        case settings_t::layouter_t::kde: {
            #if ENABLE_KDE
                layouter = layouter_kde_create();
            #endif // ENABLE_KDE
        } break;
        case settings_t::layouter_t::xkb: {
            #if WITH_X
                layouter = layouter_xkb_create();
            #endif // WITH_X
        } break;
    };
    if ( not layouter ) {
        ERR( "No layouters available." );
    };
    return layouter;
};

time_t
layouter_t::repeat_delay(
) {
    if ( not _repeat_delay_ ) {
        _repeat_delay_ = _get_repeat_delay();
        if ( not _repeat_delay_ ) {
            _repeat_delay_ = 500;
        };
    };
    return _repeat_delay_;
};

layout_range_t
layouter_t::layout_range(
) {
    return layout_range_t();
};

layouts_t
layouter_t::layouts(
) {
    layouts_t result;
    auto names = _layout_names();
    for ( size_t i = 0, end = names.size(); i < end; ++ i ) {
        if ( not names[ i ].empty() ) {
            result.insert( layout_t( i + 1 ) );
        };
    };
    return result;
};

string_t
layouter_t::layout_name(
    layout_t layout
) {
    layout_range().check( layout );
    auto i = layout.index;
    auto names = _layout_names();
    return 1 <= i and size_t( i ) <= names.size() ? names[ i - 1 ] : "";
};

string_t
layouter_t::layout_full_name(
    layout_t layout
) {
    auto name = layout_name( layout );
    return str( layout.index ) + ( name.empty() ? "" : ":" + name );
};

layout_t
layouter_t::layout(
    string_t const & name
) {
    auto names = _layout_names();
    for ( size_t i = 0, end = names.size(); i < end; ++ i ) {
        if ( names[ i ] == name ) {
            return layout_t( i + 1 );
        };
    };
    return layout_t();
};

void
layouter_t::start(
    bool        bell,
    on_event_t  handler
) {
    if ( bell and not _can_ring() ) {
        INF( type() << " layouter cannot ring the bell." );
    };
    _bell_ = bell;
    _handler = handler;
    _start();
}; // start

bool
layouter_t::bell(
) {
    return _bell_;
}; // bell

void
layouter_t::stop(
) {
    _stop();
    _bell_ = false;
    _handler = nullptr;
}; // stop

time_t
layouter_t::_get_repeat_delay(
) {
    return 0;
};

strings_t
layouter_t::_get_layout_names(
) {
    return strings_t();
};

bool
layouter_t::_can_ring(
) {
    return false;
};

strings_t const &
layouter_t::_layout_names(
) {
    if ( not _layout_names_ ) {
        _layout_names_.reset( new strings_t( _get_layout_names() ) );
    };
    return * _layout_names_.get();
};

}; // namespace tapper

// end of file //
