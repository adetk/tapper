/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/app.hpp

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
    `app_t` class interface.

    @sa app.cpp
**/

#ifndef _TAPPER_APP_HPP_
#define _TAPPER_APP_HPP_

#include "base.hpp"

#include "emitter.hpp"
#include "layouter.hpp"
#include "listener.hpp"
#include "settings.hpp"
#include "types.hpp"

struct argp_state;  // instead of #include <argp.h>.

namespace tapper {

/**
    Application class.

    Application parses command line options and arguments, loads and saves settings, basing on
    configuration, selects and creates listener, layouter, and emitter, creates and runs tapper or
    prints list of known keys or list of known layouts.
**/
class app_t: public object_t {

    public:

        /** Application exit status. **/
        enum exit_status_t {
            ok,                 ///< Everything is ok.
            not_ok,             ///< False, but not an error. Not actually used.
            cmdline_error,      ///< Usage error, something wrong in the command line.
            runtime_error,      ///< Runtime error.
        };

        /** Application exception class. **/
        class error_t: public std::runtime_error {
            public:
                using std::runtime_error::runtime_error;
        }; // class error_t

        /**
            Constructor, `app_t` constructor. Contructor does everything. `main()` should only
            create an instance of `app_t`, all the stuff will be done by the constructor.

            Currently contructor reports runtime errors by throwing exceptions. Glibc argp is used
            to parse the command line, argp parser calls `exit()` in case of command-line errors.
        **/
        app_t( int argc, char * argv[] );

        ~app_t();

    private:

        /** Working mode. **/
        enum class mode_t {
            dflt,           ///< Default mode.
            run = dflt,     ///< Run tapper and let it work until `SIGINT` or `SIGTERM`, then exit.
            autostart,      ///< Add Tapper to autostart and exit.
            dconf_editor,   ///< Run dconf-editor and exit.
            lay_off,        ///< Exit after parsing the command line.
            list_keys,      ///< List known keys and exit.
            list_layouts,   ///< List known layouts and exit.
            no_autostart,   ///< Remove Tapper from autostart and exit.
            reset_settings, ///< Reset settings to default state.
            save_settings,  ///< Save settings to GSettings and exit.
            show_taps,      ///< Show detected taps instead of normal work.
        };

        void set_locale();

        // Command line parsing routines:
        void             parse_cmdline( int argc, char * argv[] );
        static ::error_t parse_opt_or_arg( int key, char * _arg, argp_state * state );
        void             parse_assignment( string_t const & string );
        key_t            parse_key( string_t const & string );
        actions_t        parse_actions( string_t const & string );
        actions_t        parse_action( string_t const & string );
        layout_t         parse_layout( string_t const & string );

        // Low-level actions, called from parsing routines:
        void set_listener( settings_t::listener_t listener );
        void set_layouter( settings_t::layouter_t layouter );
        void set_emitter( settings_t::emitter_t emitter );
        void set_bell( settings_t::bell_t bell );
        void set_mode( mode_t mode );
        void load_settings();
        void reset_settings();
        void save_settings();

        // Top-level actions:
        void autostart( bool enable = true );
        void dconf_editor();
        void lay_off();
        void list_keys();
        void list_layouts();
        void run();

        void print_intro();

        string_t to_string( actions_t const & actions );

        string_t layout_name( layout_t layout );
        string_t key_name( key_t key );

        listener_t &        listener();
        layouter_t &        layouter();
        emitter_t &         emitter();
        settings_t::bell_t  bell();

    private:

        char const * const  prog_name { nullptr };          ///< Program name.

        settings_t          _settings;                      ///< Current settings.
        bool                _load_settings { WITH_GLIB };
            ///< If true, the app will load setting from GSettings, otherwise it won't.
        bool                _quiet { false };
            ///< If true, the app won't print intro message.
        bool                _syslog { false };
            ///< If true, copy error and warning messages to syslog.
        bool                _use_default_assignments { true };
            ///< If true, default key assignmnets are enabled.
        mode_t              _mode { mode_t::dflt };         ///< Working mode.
        bool                _cmdline_parsed { false };
            ///< `true`, if `parse_cmdline()` has finished, `false` otherwise.

        keys_p              _used_keys;
            ///< Set of keys used in the command line to detect keys assigned more than once.

        listener_p          _listener;
        layouter_p          _layouter;
        emitter_p           _emitter;

}; // class app_t

template<>
inline
string_t
str(
    app_t::exit_status_t const & value
) {
    return std::to_string( int( value ) );
};

}; // namespace tapper

#endif // _TAPPER_APP_HPP_

// end of file //
