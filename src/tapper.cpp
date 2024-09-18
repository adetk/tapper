/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/tapper.cpp

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
    `tapper_t` class implementation.
**/

#include "tapper.hpp"

namespace tapper {

// -------------------------------------------------------------------------------------------------
// tapper_t
// -------------------------------------------------------------------------------------------------

tapper_t::tapper_t(
    listener_t &          listener,
    layouter_t &          layouter,
    emitter_t &           emitter
):
    OBJECT_T(),
    _listener( listener ),
    _layouter( layouter ),
    _emitter( emitter ),
    _repeat_delay( _layouter.repeat_delay() ),
    _key_range( _listener.key_range() ),
    _key_state( uint_t( _key_range.max ) + 1, false )
{
    if ( _repeat_delay == 0 ) {
        _repeat_delay = 500;
    };
    INF( "Repeat delay: " << _repeat_delay << " ms" );
}; // ctor

tapper_t::~tapper_t(
) {
    // TODO: stop listener?
}; // dtor

void
tapper_t::start(
    assignments_t const & assignments,
    bool                  bell,
    bool                  show_taps
) {
    _assignments = assignments;
    _show_taps = show_taps;
    keys_t keys;
    for ( auto const & assignment: _assignments ) {
        for ( auto const action: assignment.second ) {
            if ( action.type() != action_t::type_t::activate_layout ) {
                keys.insert( action.key() );
            };
        };
    };
    _emitter.start( keys );
    _layouter.start(
        bell,
        [ this ]( bool active ) { _active = active; }
        /*
            If user session is not active, deactivate tapper as well, and activate tapper when the
            user session becomes active.
        */
    );
    _listener.start( std::bind( & tapper_t::_on_event, this, std::placeholders::_1 ) );
}; // start

void
tapper_t::stop(
) {
    _listener.stop();
    _layouter.stop();
    _emitter.stop();
}; // stop

void
tapper_t::_on_event(
    event_t const & event
) {
    TRACE();
    if ( _key_range.includes( event.key.code() ) ) {
        if ( event.state == key_state_t::pressed ) {
            if ( _key_state[ int_t( event.key.code() ) ] ) {
                DBG( _pressed_keys << "↓ ~" << event.key );
                if ( event.key == _last_key ) {
                    /*
                        Look like key press is autorepeating. Do I have autorepeat timeout for
                        free? Nope. libinput does not report autorepeating at all. XRecord does,
                        but it also could be a key on *another* keyboard. It is not very common,
                        but real case.
                    */
                }; // if
                /*
                    No need in updating keyboard state -- we already know the key is pressed.
                    Number of pressed keys is not changed too. But let us reset the last pressed
                    key -- if autorepeating takes place this is not a tap.
                */
                _last_key = key_t();
            } else {
                // Update keyboard state:
                DBG( _pressed_keys << "↓ +" << event.key );
                _key_state[ event.key.code() ] = true;
                _pressed_keys += 1;
                _last_key      = event.key;
                _pressed_at    = event.time;
            }; // if
        } else {
            DBG( _pressed_keys << "↓ -" << event.key );
            /*
                This check is a must because unconditional decreasing `_pressed_keys` may cause
                wrong result. For example, if the program started from the command line, the first
                received event will likely be releasing of Enter key.
            */
            if ( _key_state[ event.key.code() ] ) {
                _key_state[ event.key.code() ] = false;
                assert( _pressed_keys > 0 );
                -- _pressed_keys;
            };
            if (
                event.key == _last_key
                and _pressed_keys == 0
                and event.time - _pressed_at <= _repeat_delay
            ) {
                DBG( "⇵" << event.key );
                _on_tap( event.key );
            }; // if
            _last_key = key_t();
        }; // if
    } else {
        WRN(
            "Listener reported " << event.key << " out of expected range " << _key_range << "."
        );
    }; // if
};

void
tapper_t::_on_tap(
    key_t  key
) {
    TRACE();
    if ( not _active ) {
        return;
    };
    if ( _show_taps ) {
        OUT( "Key " << _listener.key_full_name( key ) << " tapped." );
        return;
    };
    auto it = _assignments.find( key );
    if ( it != _assignments.end() ) {
        for ( auto const & action: it->second ) {
            DBG( "Executing action " << action << "…" );
            switch ( action.type() ) {
                case action_t::type_t::none: {
                } break;
                case action_t::type_t::activate_layout: {
                    _layouter.activate( action.layout() );
                } break;
                case action_t::type_t::emit_key_tap: {
                    _emitter.emit( {
                        { action.key(), key_state_t::pressed  },
                        { action.key(), key_state_t::released },
                    } );
                } break;
            };
        };
    };
};

}; // namespace tapper

// end of file //
