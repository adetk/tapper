/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/app.cpp

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
    `app_t` class implementation.

    @sa app.hpp
**/

#include "app.hpp"

#include <argp.h>

#if WITH_GLIB
    #include <glibmm/init.h>
    #include <giomm/init.h>
#endif // WITH_GLIB

#include "posix.hpp"
#include "privileges.hpp"
#include "string.hpp"
#include "tapper.hpp"
#include "xdg.hpp"

// Definition of argp variables must be in global namespace.

/**
    Program version string. It is printed in respond to `--version` option. The string may include
    copyright.
**/
char const * argp_program_version =
    PACKAGE_STRING ".\n"
    "Copyright (C) " PACKAGE_COPYRIGHT_YEAR " " PACKAGE_COPYRIGHT_HOLDER ".\n"
    "License " PACKAGE_LICENSE " <" PACKAGE_LICENSE_URL ">.\n"
    "This is free software: you are free to change and redistribute it." "\n"
    "There is NO WARRANTY, to the extent permitted by law."
    ;

/** Email address to send bug reports to. May be `nullptr`. **/
char const * argp_program_bug_address = nullptr; /*
    argp separates "Report bugs to" from preceeding text with empty line. I do not like it.
    Let's reset bug report address, I'll include it into help text manually.
*/

/** Status to return in case of command line error. **/
error_t argp_err_exit_status = tapper::app_t::cmdline_error;

namespace tapper {

// -------------------------------------------------------------------------------------------------
// app_t
// -------------------------------------------------------------------------------------------------

static void handler( int signal ) {
    DBG( "Signal " << signal );
};

static assignments_t const default_assignments {
    { key_t( 29 ), { action_t::activate_layout( layout_t( 1 ) ) } }, // Left Ctrl activates the 1st layout.
    { key_t( 97 ), { action_t::activate_layout( layout_t( 2 ) ) } }, // Right Ctrl activates the 2nd layout.
};

app_t::app_t(
    int     argc,
    char *  argv[]
):
    OBJECT_T(),
    prog_name( program_invocation_short_name )
{

    set_locale();
    privileges().init();

    #if WITH_GLIB
        /*
            Lazy initialization is too complicated, because it is hot clear if these functions
            are thread-safe, so let's initialize glibmm and giomm libraries here.
        */
        Glib::init();
        Gio::init();
    #endif // WITH_GLIB

    parse_cmdline( argc, argv );
    if ( _syslog ) {
        set_syslog_min_priority( priority_t::warning );
    };

    posix::signal::set_t    set( { SIGINT, SIGTERM } );
    posix::signal::action_t action( handler, set );
    posix::signal::action( SIGINT,  action );
    posix::signal::action( SIGTERM, action );

    switch ( _mode ) {
        case mode_t::autostart: {
            autostart();
        } break;
        case mode_t::dconf_editor: {
            dconf_editor();
        } break;
        case mode_t::lay_off: {
            lay_off();
        } break;
        case mode_t::list_keys: {
            list_keys();
        } break;
        case mode_t::list_layouts: {
            list_layouts();
        } break;
        case mode_t::reset_settings: {
            reset_settings();
        } break;
        case mode_t::show_taps:
        case mode_t::run: {
            run();
        } break;
        case mode_t::save_settings: {
            save_settings();
        } break;
        case mode_t::no_autostart: {
            autostart( false );
        } break;
    }; // switch

}; // ctor

app_t::~app_t(
) {
    /*
        `_listener` and `_layouter` will be reset automatically, but I want to print "Done" after
        destructing them, not before.
    */
    _listener.reset();
    _layouter.reset();
    _emitter.reset();
    DBG( "Done." );
};

void
app_t::set_locale(
) {
    // TODO: Add prefix "Can't set locale: ", because otherwise warning is too cryptic.
    //~ CATCH_ALL( void( std::locale::global( std::locale( "" ) ) ) );
};

/** Command line options. **/
enum opt_t {
    // Short options:
    opt_x = 'x',
    // Long options:
    opt_autostart = 1000,
    opt_bell,
    opt_dconf_editor,
    opt_emitter,
    opt_gnome,
    opt_kde,
    opt_lay_off,
    opt_layouter,
    opt_libevdev,
    opt_libinput,
    opt_listener,
    opt_list_keys,
    opt_list_layouts,
    opt_load_settings,
    opt_no_autostart,
    opt_no_bell,
    opt_no_default_assignments,
    opt_no_load_settings,
    opt_quiet,
    opt_reset_settings,
    opt_save_settings,
    opt_show_taps,
    opt_syslog,
    opt_xkb,
    opt_xrecord,
    opt_xtest,
}; // enum opt_t

/**
    Parses one command line option or argument. The method is static because it is called from
    within `argp_parse()`.
**/
::error_t
app_t::parse_opt_or_arg(
    int             key,        ///< Option key (a short name or integer id).
    char *          _arg,       ///< String to parse.
    argp_state *    state       ///< Parser state == pointer to `app_t` object.
) {

    char const * arg = _arg; /*
        It is a little trick. I am not going to change argument, but parameter should be of `char
        *` type.
    */

    auto app = static_cast< app_t * >( state->input );

    // cppcheck-suppress shadowFunction
    THIS( app );

    try {       // No exceptions should leak from this method.

        switch ( key ) {

            case ARGP_KEY_INIT: {
                DBG( "ARGP_KEY_INIT " << state->next << " " << state->argc );
                app->_used_keys.reset( new keys_t );
            } break;

            case opt_autostart: {
                app->set_mode( mode_t::autostart );
            } break;

            case opt_bell: {
                app->set_bell( settings_t::bell_t::enabled );
            } break;

            case opt_dconf_editor: {
                if ( not WITH_GLIB ) {
                    ERR( "Program is built without GLib." );
                };
                app->set_mode( mode_t::dconf_editor );
            } break;

            case opt_emitter: {
                app->set_emitter( val< settings_t::emitter_t >( arg ) );
            } break;

            case opt_gnome: {
                // cppcheck-suppress unknownMacro; cppchek 2.3 complains on 'not'!?
                if ( not ENABLE_GNOME ) {
                    ERR( "Program is built without GNOME support." );
                }; // if
                app->set_layouter( settings_t::layouter_t::gnome );
            } break;

            case opt_kde: {
                // cppcheck-suppress unknownMacro; cppchek 2.3 complains on 'not'!?
                if ( not ENABLE_KDE ) {
                    ERR( "Program is built without KDE support." );
                }; // if
                app->set_layouter( settings_t::layouter_t::kde );
            } break;

            case opt_lay_off: {
                app->set_mode( mode_t::lay_off );
            } break;

            case opt_layouter: {
                app->set_layouter( val< settings_t::layouter_t >( arg ) );
            } break;

            case opt_libinput: {
                if ( not WITH_LIBINPUT ) {
                    ERR( "Program is built without libinput." );
                };
                app->set_listener( settings_t::listener_t::libinput );
            } break;

            case opt_libevdev: {
                if ( not WITH_LIBEVDEV ) {
                    ERR( "Program is built without libevdev." );
                };
                app->set_emitter( settings_t::emitter_t::libevdev );
            } break;

            case opt_listener: {
                app->set_listener( val< settings_t::listener_t >( arg ) );
            } break;

            case opt_list_keys: {
                app->set_mode( mode_t::list_keys );
            } break;

            case opt_list_layouts: {
                app->set_mode( mode_t::list_layouts );
            } break;

            case opt_load_settings: {
                if ( not WITH_GLIB ) {
                    ERR( "Program is built without GLib." );
                };
                app->_load_settings = true;
            } break;

            case opt_no_autostart: {
                app->set_mode( mode_t::no_autostart );
            } break;

            case opt_no_bell: {
                app->set_bell( settings_t::bell_t::disabled );
            } break;

            case opt_no_default_assignments: {
                app->_use_default_assignments = false;
            } break;

            case opt_no_load_settings: {
                app->_load_settings = false;
            } break;

            case opt_quiet: {
                app->_quiet = true;
            } break;

            case opt_reset_settings: {
                if ( not WITH_GLIB ) {
                    ERR( "Program is built without GLib." );
                };
                app->set_mode( mode_t::reset_settings );
            } break;

            case opt_save_settings: {
                if ( not WITH_GLIB ) {
                    ERR( "Program is built without GLib." );
                };
                app->set_mode( mode_t::save_settings );
            } break;

            case opt_show_taps: {
                app->set_mode( mode_t::show_taps );
                app->set_emitter( settings_t::emitter_t::dummy );
            } break;

            case opt_syslog: {
                app->_syslog = true;
            } break;

            case opt_x: {
                if ( not WITH_X ) {
                    ERR( "Program is built without X Window System." );
                };
                app->set_listener( settings_t::listener_t::xrecord );
                app->set_layouter( settings_t::layouter_t::xkb );
                app->set_emitter( settings_t::emitter_t::xtest );
            } break;

            case opt_xkb: {
                if ( not WITH_X ) {
                    ERR( "Program is built without X Window System." );
                };
                app->set_layouter( settings_t::layouter_t::xkb );
            } break;

            case opt_xrecord: {
                if ( not WITH_X ) {
                    ERR( "Program is built without X Window System." );
                };
                app->set_listener( settings_t::listener_t::xrecord );
            } break;

            case opt_xtest: {
                if ( not WITH_X ) {
                    ERR( "Program is built without X Window System." );
                };
                app->set_emitter( settings_t::emitter_t::xtest );
            } break;

            case ARGP_KEY_NO_ARGS: {
                DBG( "ARGP_KEY_NO_ARGS" );
                // Do not load settings just to reset them.
                // TODO: Should I parse arguments, if an option does not need it? E. g.
                // --reset-settings or --list-keys?
                if ( app->_load_settings and app->_mode != mode_t::reset_settings ) {
                    app->load_settings();
                };
                if ( app->_settings.assignments.empty() and app->_use_default_assignments ) {
                    app->_settings.assignments = default_assignments;
                };
            } break;

            case ARGP_KEY_ARG: {
                DBG( "ARGP_KEY_ARG " << state->next << " " << state->argc );
                if ( state->arg_num == 0 ) {
                    if ( not (
                        app->_mode == mode_t::run or
                        app->_mode == mode_t::lay_off or
                        app->_mode == mode_t::save_settings
                    ) ) {
                        ERR( "Assignments are not allowed in this mode." );
                            // TODO: Replace "this" with mode name.
                    };
                    /*
                        Load settings just before processing the first argument, because settings (e.
                        g. listener) may affect argument parsing.
                    */
                    if ( app->_load_settings ) {
                        app->load_settings();
                    };
                };
                app->parse_assignment( arg );
            } break;

            case ARGP_KEY_SUCCESS: {
                DBG( "ARGP_KEY_SUCCESS "  << state->next << " " << state->argc );
            } break;

            case ARGP_KEY_ERROR: {
                DBG( "ARGP_KEY_ERROR " << state->next << " " << state->argc );
            } break;

            case ARGP_KEY_END: {
                DBG( "ARGP_KEY_END " << state->next << " " << state->argc );
                /*
                    Command line parsing ends, I don't need _used_keys any more. Let's free a bit
                    of memory.
                */
                app->_used_keys.reset();
            } break;

            case ARGP_KEY_FINI: {
                DBG( "ARGP_KEY_FINI " << state->next << " " << state->argc );
            } break;

            default: {
                DBG( "ARGP_KEY_DEFAULT " << state->next << " " << state->argc );
                return ARGP_ERR_UNKNOWN;
            } break;

        }; // switch

    } catch ( std::exception const & ex ) {

        char const * format = nullptr;
        switch ( key ) {
            case ARGP_KEY_ARG: {
                format = "Bad argument %s: %s";
            } break;
            case ARGP_KEY_NO_ARGS: {
                std::rethrow_exception( std::current_exception() );
            } break;
            default: {      // TODO: It seems it does not work.
                format = "Bad option %s: %s";
                arg = state->argv[ state->next - 1 ];
            } break;
        }; // switch
        argp_error( state, format, q( arg ).c_str(), ex.what() );

    }; // try

    return 0;

}; // parse_opt_or_arg

/**
    Parses entire command line. Command line is parsed with Glibc argp parser.
**/
void
app_t::parse_cmdline(
    int     argc,
    char *  argv[]
) {

    TRACE();
    DBG( "Parsing command line…" );

    int const doc_opt      = OPTION_DOC | OPTION_NO_USAGE;
    int const gnome_opt    = ENABLE_GNOME  ? 0 : OPTION_HIDDEN;
    int const kde_opt      = ENABLE_KDE    ? 0 : OPTION_HIDDEN;
    int const glib_opt     = WITH_GLIB     ? 0 : OPTION_HIDDEN;
    int const x_opt        = WITH_X        ? 0 : OPTION_HIDDEN;
    int const libevdev_opt = WITH_LIBEVDEV ? 0 : OPTION_HIDDEN;
    int const libinput_opt = WITH_LIBINPUT ? 0 : OPTION_HIDDEN;

    string_t const listener_help =
        "Listener to use, one of: " + join( ", ", settings_t::listeners() );
    string_t const layouter_help =
        "Layouter to use, one of: " + join( ", ", settings_t::layouters() );
    string_t const emitter_help =
        "Emitter to use, one of: " + join( ", ", settings_t::emitters() );

    argp_option opts[] = {
        { "Listener selection:", 0,                             nullptr,    doc_opt,
            "",
            100 },
        { "listener",               opt_listener,               "LISTENER", 0,
            listener_help.c_str(),
            101 },
        { "libinput",               opt_libinput,               nullptr,    libinput_opt,
            "Same as --listener=libinput",
            102 },
        { "xrecord",                opt_xrecord,                nullptr,    x_opt,
            "Same as --listener=xrecord",
            103 },

        { "Layouter selection:",    0,                          nullptr,    doc_opt,
            "",
            200 },
        { "layouter",               opt_layouter,               "LAYOUTER", 0,
            layouter_help.c_str(),
            201 },
        { "gnome",                  opt_gnome,                  nullptr,    gnome_opt,
            "Same as --layouter=gnome",
            202 },
        { "kde",                    opt_kde,                    nullptr,    kde_opt,
            "Same as --layouter=kde",
            203 },
        { "xkb",                    opt_xkb,                    nullptr,    x_opt,
            "Same as --layouter=xkb",
            204 },

        { "Emitter selection:",     0,                          nullptr,    doc_opt,
            "",
            300 },
        { "emitter",                opt_emitter,                "EMITTER",  0,
            emitter_help.c_str(),
            301 },
        { "libevdev",               opt_libevdev,               nullptr,    libevdev_opt,
            "Same as --emitter=libevdev",
            302 },
        { "xtest",                  opt_xtest,                  nullptr,    x_opt,
            "Same as --emitter=xtest",
            303 },

        { "Bell mode:",             0,                          nullptr,    doc_opt,
            "",
            400 },
        { "bell",                   opt_bell,                   nullptr,    0,
            "Ring a bell when a layout is activated",
            401 },
        { "no-bell",                opt_no_bell,                nullptr,    0,
            "Do not ring a bell when a layout is activated",
            402 },
        /*
            cppcheck warns:

                Operator '|' with one operand equal to zero is redundant.

            However, depending on the configuration `glib` may be zero or non-zero.
        */
        // cppcheck-suppress badBitmaskCheck
        { "Settings management:",   0,                          nullptr,    doc_opt | glib_opt,
            "",
            500 },
        { "dconf-editor",           opt_dconf_editor,           nullptr,    0,
            "Execute dcong-editor to edit Tapper settings",
            501 },
        { "load-settings",          opt_load_settings,          nullptr,    glib_opt,
            "Load settings (listener, layouter, emitter, bell mode, key assignments)",
            502 },
        { "no-load-settings",       opt_no_load_settings,       nullptr,    glib_opt,
            "Do not load settings",
            503 },
        { "reset-settings",         opt_reset_settings,         nullptr,    glib_opt,
            "Reset settings and exit",
            504 },
        { "save-settings",          opt_save_settings,          nullptr,    glib_opt,
            "Save settings and exit",
            505 },

        { "Other options:",         0,                          nullptr,    doc_opt,
            "",
            600 },
        { "autostart",              opt_autostart,              nullptr,    0,
            "Set up Tapper to start automatically when you log in and exit",
            601 },
        { "no-autostart",           opt_no_autostart,           nullptr,    0,
            "Cancel Tapper autostart and exit",
            601 },
        { "lay-off",                opt_lay_off,                nullptr,    0,
            "Parse command line and exit",
            602 },
        { "no-default-assignments", opt_no_default_assignments, nullptr,    0,
            "Do not use default assignments",
            603 },
        { "quiet",                  opt_quiet,                  nullptr,    0,
            "Do not print introductory messages",
            604 },
        { "syslog",                 opt_syslog,                 nullptr,    0,
            "Copy run time errors and warnings to system log",
            605 },
        { "x",                      opt_x,                      nullptr,    x_opt,
            "Same as --xrecord --xkb --xtest",
            606 },

        { "Help options:",          0,                          nullptr,    doc_opt,
            "",
            -2  },
        { "list-keys",              opt_list_keys,              nullptr,    0,
            "List available keys and exit",
            -1  },
        { "list-layouts",           opt_list_layouts,           nullptr,    0,
            "List available layouts and exit",
            -1  },
        { "show-taps",              opt_show_taps,              nullptr,    0,
            "Print key code of every tapped key",
            -1  },

        { 0 }

    }; // opts

    strings_t const disabled = {
        #if ! ENABLE_GNOME
            "GNOME",
        #endif
        #if ! ENABLE_KDE
            "KDE",
        #endif
    };
    strings_t const without = {
        #if ! WITH_GLIB
            "GLib",
        #endif
        #if ! WITH_LIBEVDEV
            "libevdev",
        #endif
        #if ! WITH_LIBINPUT
            "libinput",
        #endif
        #if ! WITH_X
            "X Window System",
        #endif
    };

    // TODO: Show default assignments.
    string_t const doc = STR(
        "\n"
        PACKAGE_NAME " — " PACKAGE_SUMMARY ". "
        PACKAGE_NAME " works in the background, "
        "when an assigned KEY is tapped, " PACKAGE_NAME " executes the corresponding ACTIONs."
        "\n\n"
        "KEY is either a key code (an integer number in range " << key_t::range_t() << ") "
        "or a symbolic key name. "
        "Use --list-keys option to see the list of keys. "
        "Key names depend on selected listener while key codes do not."
        "\n\n"
        "\"Tap\" means a single key is pressed and quickly released. "
        "If the key is released slowly, "
        "or pressed/released in a combination with other key(s) and/or button(s), it is not a tap."
        "\n\n"
        "ACTION is either: "
        "@LAYOUT — command to activate the given layout or "
        "KEY — command to simulate tap on the given key."
        "\n\n"
        "LAYOUT is either layout index "
        "(an integer number in range " << layout_range_t() << ") or layout name. "
        "Use --list-layouts option to see the list of keyboard layouts. "
        "Layouts are layouter-dependent."
        "\n\n"
        << (
            WITH_GLIB ?
                "By default " PACKAGE_NAME " load its configuration "
                "(listener, layouter, and emitter selections, bell mode, key assignments) "
                "from GSettings database. "
                "Command line options and key assignments override settings."
                "\n\n"
            :
                ""
        ) <<
        "Options:"
        "\v"    // Options will be listed here.
        "Exit status:\n"
        "  " << ok << " if everything is ok,\n"
        "  " << cmdline_error << " in case of usage (command-line) error,\n"
        "  " << runtime_error << " in case of runtime error.\n"
        "\n"
        "See " << prog_name << "(1) man page for more details.\n"
        << (
            disabled.empty() ?
            "" :
            "Following " PACKAGE_NAME " features are disabled: " + join( ", ", disabled ) + ".\n"
        )
        << (
            without.empty() ?
            "" :
            PACKAGE_NAME " is built without " + join( ", ", without ) + ".\n"
        ) <<
        PACKAGE_NAME " home page: <" PACKAGE_URL ">.\n"
        "Report bugs to: <" PACKAGE_BUGREPORT ">."
    );

    /*
        If `COLUMNS` environment variable set, use it. But Do not let `rmargin` be too small — it
        can cause infinite loop, see <https://sourceware.org/bugzilla/show_bug.cgi?id=2016>.
    */
    auto rmargin = std::max( 80, std::atoi( posix::get_env( "COLUMNS" ).c_str() ) );
    posix::set_env(
        "ARGP_HELP_FMT",
        STR(
            "short-opt-col=4," <<
            "long-opt-col=8," <<
            "opt-doc-col=33," <<
            "rmargin=" << rmargin << "," <<
            //~ posix::get_env( "ARGP_HELP_FMT" ) <<
            /*
                Appending original value of `ARGP_HELP_FMT` to the end looks like a good idea: it
                let user tune help formatting. Unfortunately, arpg help formatter has known bug
                (<https://sourceware.org/bugzilla/show_bug.cgi?id=2016>) causing either infinite
                loop or crash. To avoid it, it is better ignore potentially unsafe `ARGP_HELP_FMT`
                and use only verified hardcoded options.
            */
            ""
        )
    );

    argp parser = { opts, parse_opt_or_arg, "[KEY=[ACTION,...]...]", doc.c_str() };
    auto error = argp_parse( & parser, argc, argv, 0, 0, this );
    if ( error ) {
        // Actually, this does not work, since `argp_parse` calls `exit` internally.
        using error_t = posix::error_t;
        ERR( "Can't parse command line", error );
    }; // if

    _cmdline_parsed = true;

}; // parse_argv

/** Parses one assignment. **/
void
app_t::parse_assignment(
    string_t const & string
) {
    TRACE();
    auto parts   = split( '=', string, 2 );
    auto key     = parse_key( trim( parts[ 0 ] ) );
    auto actions = parts.size() > 1 ? parse_actions( parts[ 1 ] ) : actions_t();
    // Check if the key is already assigned:
    auto inserted = _used_keys->insert( key );
    if ( not inserted.second ) {    // key was not inserted because it already is in the set.
        ERR( "Key " << key_name( key ) << " has been already assigned." );
    }; // if
    // Save the assignment in the settings:
    if ( actions.empty() ) {
        _settings.assignments.erase( key );     // Cancel key assignment, if any.
    } else {
        _settings.assignments[ key ] = actions;
    };
}; // parse_assignment

/** Parses key, which can be either key name or key code. **/
key_t
app_t::parse_key(
    string_t const & string
) {
    TRACE();
    using error_t = val_error_t;
    string_t subject = "Bad key";
    try {
        auto result = key_t();
        if ( string.empty() ) {
            ERR( "No key." );
        }; // if
        subject = "Bad key code";
        if ( like_cardinal( string ) ) {    // Looks like key code.
            result = key_t( val< key_t::rep_t >( string ) );
            listener().key_range().check( result ); // TODO: static or dynamic range?
        } else if ( like_integer( string ) ) {
            ERR( "Signed key code is not allowed in this context." );
        } else {                            // Looks like layout name.
            subject = "Bad key name";
            result = listener().key( string );
            if ( not result.code() ) {
                ERR( "No such key." );
            };
        }; // if
        return result;
    } catch ( error_t const & ex ) {
        ERR( subject << " " << q( string ) << ": " << ex.what() );
    };
}; // parse_key

/** Parses comma-separated list of actions. **/
actions_t
app_t::parse_actions(
    string_t const & string
) {
    actions_t result;
    // TODO: Layout name may include comma.
    for ( auto action: split( ',', string ) ) {
        action = trim( action );
        auto actions = parse_action( action );
        std::copy( actions.begin(), actions.end(), std::back_inserter( result ) );
    };
    // TODO: Check there is no more than one layout action.
    // TODO: Check there are no loops in emit actions.
    // TODO: Check all pressed keys are released?
    return result;
}; // parse_actions

/**
    Parses action. Action is either `@`*layout* or *key*. *layout* can be either layout index or
    name, *key* can be either key code or key name.
**/
actions_t
app_t::parse_action(
    string_t const & string
) {
    using error_t = val_error_t;
    actions_t result;
    try {
        if ( string.empty() ) {
        } else if ( front( string ) == '@' ) {
            result.push_back( action_t::activate_layout( parse_layout( string.substr( 1 ) ) ) );
        } else {
            result.push_back( action_t::emit_key_tap( parse_key( string ) ) );
        };
    } catch ( error_t const & ex ) {
        ERR( "Bad action " << q( string ) << ": " << ex.what() );
    };
    return result;
}; // parse_action

/** Parses layout, which can be either layout name or number. **/
layout_t
app_t::parse_layout(
    string_t const & string
) {
    using error_t = val_error_t;
    string_t subject = "Bad layout";
    try {
        layout_t result;
        if ( string.empty() ) {
            ERR( "No layout." );
        }; // if
        subject = "Bad layout index";
        if ( like_cardinal( string ) ) {     // Looks like layout index.
            result.index = val< layout_t::rep_t >( string );
            layouter().layout_range().check( result ); // TODO: static or dynamic range?
        } else {                            // Looks like layout name.
            subject = "Bad layout name";
            result = layouter().layout( string );
            if ( not result.index ) {
                ERR( "No such layout." );
            };
        }; // if
        return result;
    } catch ( error_t const & ex ) {
        ERR( subject << " " << q( string ) << ": " << ex.what() );
    };
}; // parse_layout

void
app_t::set_listener(
    settings_t::listener_t listener
) {
    if ( _settings.listener != settings_t::listener_t::unset ) {
        ERR( "Listener has already been set." );
    };
    _settings.listener = listener;
};

void
app_t::set_layouter(
    settings_t::layouter_t layouter
) {
    if ( _settings.layouter != settings_t::layouter_t::unset ) {
        ERR( "Layouter has already been set." );
    };
    _settings.layouter = layouter;
};

void
app_t::set_emitter(
    settings_t::emitter_t emitter
) {
    if ( _settings.emitter != settings_t::emitter_t::unset ) {
        ERR( "Emitter has already been set." );
    };
    _settings.emitter = emitter;
};

void
app_t::set_bell(
    settings_t::bell_t bell
) {
    if ( _settings.bell != settings_t::bell_t::unset ) {
        ERR( "Bell has already been set." );
    };
    _settings.bell = bell;
};

void
app_t::set_mode(
    mode_t mode
) {
    if ( _mode != mode_t::dflt ) {
        ERR( "Mode has already been set." );
    };
    _mode = mode;
};

/**
    Load settings from GSettings database and combine loaded settings with current settings.
    Current settings have priority over the loaded settings.
**/
void
app_t::load_settings(
) {
    DBG( "Loading settings…" );
    /*
        Settings extracted from the command line have priority over settings loaded from GSettings,
        so loaded settings should be on the left side of `+`.
    */
    _settings = settings_t::load() + _settings;
};

/** Resets Tapper settings in GSettings database to default values. **/
void
app_t::reset_settings(
) {
    DBG( "Resetting settings…" );
    settings_t::reset();
};

/** Saves current settings to GSettings database. **/
void
app_t::save_settings(
) {
    DBG( "Saving settings…" );
    _settings.save();
};

/** Manage Tapper autostart. **/
void
app_t::autostart(
    bool enable     ///< if `true`, autostart enabled, otherwise autostart disabled.
) {
    if ( ! _quiet ) {
        OUT( PACKAGE_STRING "." );
    };
    DBG( ( enable ? "Enabling autostart…" : "Disabling autostart…" ) );
    // Get config home:
    auto config    = xdg::config_home();
    auto desktop   = string_t( PACKAGE_ID ) + ".desktop";
    auto autostart = config + "/autostart";
    auto link      = autostart + "/" + desktop;
    try {
        posix::mkdir( autostart );
    } catch ( posix::error_t const & ex ) {
        if ( ex.error() == EEXIST ) {
            /*
                Directory already exists — it is not a problem. It could be not a directory,
                though, in such case the error will be raised a bit later.
            */
        } else {
            throw;
        };
    };
    // Remove old link:
    try {
        posix::unlink( link );
    } catch ( posix::error_t const & ex ) {
        if ( ex.error() == ENOENT ) {
            // Link does not exist — it is not a problem.
        } else {
            throw;
        };
    };
    // Create new link if required:
    if ( enable ) {
        // TODO: Create a relative symlink?
        auto target  = string_t( DATADIR ) + "/applications/" + desktop;
        posix::symlink( target, link );
    };
    if ( ! _quiet ) {
        OUT( ( enable ? "Autostart is enabled." : "Autostart is disabled." ) );
    };
};

/** Execute dconf editor to edit Tapper settings. **/
void
app_t::dconf_editor(
) {
    if ( ! _quiet ) {
        OUT( PACKAGE_STRING "." );
        OUT( "Executing dconf editor…" );
    };
    posix::execp( "dconf-editor", { PACKAGE_ID } );
};

void
app_t::lay_off(
) {
    DBG( "Laying off…" );
    print_intro();
};

/** Prints list of all known keys. **/
void
app_t::list_keys(
) {
    DBG( "Listing keys…" );
    auto keys = listener().keys();
    if ( keys.empty() ) {
        WRN(
            listener().type() << " listener does not provide any key names; "
                << "use key codes in range " << listener().key_range() << "."
        );
    } else {
        for ( auto key: keys ) {
            for ( auto const & name: listener().key_names( key ) ) {
                OUT( key << ":" << name );
            };
        }; // for
    };
}; // list_keys

/** Prints list of all known layouts. **/
void
app_t::list_layouts(
) {
    DBG( "Listing layouts…" );
    for ( auto const & layout: layouter().layouts() ) {
        OUT( layout_name( layout ) );
    }; // for
}; // list_layouts

/** Runs Tapper in normal mode. **/
void
app_t::run(
) {
    DBG( "Running tapper…" );
    print_intro();
    bool const show_taps = _mode == mode_t::show_taps;
    bool const idle = _settings.assignments.empty() and not show_taps;
    if ( not idle ) {
        if ( not show_taps ) {
            if (
                _settings.layouter == settings_t::layouter_t::dummy
                    and has_any_layout_activations( _settings.assignments )
            ) {
                WRN( "Dummy layouter will not activate layouts." );
            };
            if (
                _settings.emitter == settings_t::emitter_t::dummy
                    and has_any_key_emits( _settings.assignments )
            ) {
                WRN( "Dummy emitter will not emit keys." );
            };
        };
        auto _bell = _settings.bell == settings_t::bell_t::enabled;
        tapper_t tapper( listener(), layouter(), emitter() );
        tapper.start( _settings.assignments, _bell, show_taps );
        privileges().show();
        posix::sleep();
        tapper.stop();
    };
};

void
app_t::print_intro(
) {
    if ( ! _quiet ) {
        OUT( PACKAGE_STRING "." );
        if ( _mode == mode_t::show_taps ) {
            OUT(
                "Starting in " <<
                    listener().type() << "/" <<
                    layouter().type() << "/" <<
                    emitter().type() << " configuration."
            );
            OUT( "Tap keys to see their codes and names." );
        } else if ( _settings.assignments.empty() ) {
            OUT( "No assignments made — nothing to do." );
        } else {
            OUT(
                "Starting in " <<
                    listener().type() << "/" <<
                    layouter().type() << "/" <<
                    emitter().type() << " configuration."
            );
            OUT( "Bell is " << bell() << "." );
            for ( auto const & assignment: _settings.assignments ) {
                OUT(
                    "Tap on key " << key_name( assignment.first ) << " " <<
                        to_string( assignment.second ) << "."
                );
            };
        };
    };
};

string_t
app_t::to_string(
    actions_t const & actions
) {
    strings_t result;
    if ( actions.empty() ) {
        result.push_back( "does nothing" );
    } else {
        action_t last;
        for ( auto const & action: actions ) {
            switch ( action.type() ) {
                case action_t::type_t::none: {
                    // TODO
                } break;
                case action_t::type_t::activate_layout: {
                    auto layout = action.layout();
                    result.push_back( STR( "activates layout " << layout_name( layout ) ) );
                } break;
                case action_t::type_t::emit_key_tap: {
                    auto key = action.key();
                    result.push_back( STR( "emits tap on key " << key_name( key ) ) );
                } break;
            };
            last = action;
        };
    };
    return join( ", ", result );
};

/** Returns layout full name corresponding to the specified index. **/
string_t
app_t::layout_name(
    layout_t layout
) {
    return layouter().layout_full_name( layout );
}; // layout_name

/** Returns layout full name corresponding to the specified index. **/
string_t
app_t::key_name(
    key_t key
) {
    return listener().key_full_name( key );
}; // key_name

/**
    Returns `true` if X Window System session detected, and `false` otherwise.
**/
static
bool
is_x_session(
) {
    auto const type = posix::get_env( "XDG_SESSION_TYPE" );
    return type == "x11";
};

/**
    Returns `true` if GNOME desktop detected, and `false` otherwise. GNOME desktop includes GNOME,
    GNOME Classic and Ubuntu desktops.
**/
static
bool
is_gnome_desktop(
) {
    static strings_t const gnomes = {
        "gnome",
        "gnome-classic",
        "gnome-xorg",
        "ubuntu",
        "ubuntu-wayland",
    };
    auto const desktop = posix::get_env( "XDG_SESSION_DESKTOP" );
    return find( desktop, gnomes ) != npos;
};

/**
    Returns `true` if KDE desktop detected, and `false` otherwise.
**/
static
bool
is_kde_desktop(
) {
    auto const desktop = posix::get_env( "XDG_SESSION_DESKTOP" );
    return desktop == "KDE";
};

/**
    Creates (if not yet created) and returns listener. The method does lazy listener creation:
    listener is created when it is actually required.
**/
listener_t &
app_t::listener(
) {
    if ( not _listener ) {
        if ( _settings.listener <= settings_t::listener_t::Auto ) {
            if ( is_x_session() and WITH_X ) {
                _settings.listener = settings_t::listener_t::xrecord;
            } else if ( WITH_LIBINPUT ) {
                _settings.listener = settings_t::listener_t::libinput;
            } else if ( WITH_LIBINPUT or WITH_X ) {
                ERR( "No suitable listener." );
            } else {
                ERR( "No listeners available." );
            };
        };
        INF( "Selected listener: " << _settings.listener );
        if ( _settings.listener != settings_t::listener_t::libinput ) {
            // We can drop "input" group now.
            privileges().drop_input_group();
        };
        if ( _settings.listener == settings_t::listener_t::xrecord and not is_x_session() ) {
            WRN(
                _settings.listener << " listener selected, "
                    << "but X Window System session is not detected, "
                    << "Tapper will likely fail."
            );
        };
        _listener.reset( listener_t::create( _settings.listener ) );
    };
    return * _listener.get();
};

/**
    Creates (if not yet created) and returns layouter. The method does lazy layouter creation:
    layouter is created when it is actually required.
**/
layouter_t &
app_t::layouter(
) {
    if ( not _layouter ) {
        /*
            In "show taps" mode, the tapper will not activate layouts, so using the dummy layouter
            looks like a good idea, but… The GNOME layouter monitors the GNOME session activity. I
            do not want to allow one user to see taps made by another user, so let's choose a layouter
            even if "show taps" mode requested.
        */
        if ( _settings.layouter <= settings_t::layouter_t::Auto ) {
            if ( _cmdline_parsed and not has_any_layout_activations( _settings.assignments ) ) {
                /*
                    If there is no layout activations in assignments, I do not need a real layouter
                    and can use the dummy layouter instead. However, `layoter()` can be called
                    during command line parsing, when assignments are not yet fully known. On the
                    other side, if `layouter()` is called at command line parsing, it is called to
                    either check layout index or to convert layout name to layout index. In any
                    case it means layout activations will be in assignments.
                */
                _settings.layouter = settings_t::layouter_t::dummy;
            } else if ( is_gnome_desktop() and ENABLE_GNOME ) {
                _settings.layouter = settings_t::layouter_t::gnome;
            } else if ( is_kde_desktop() and ENABLE_KDE ) {
                _settings.layouter = settings_t::layouter_t::kde;
            } else if ( is_x_session() and WITH_X ) {
                _settings.layouter = settings_t::layouter_t::xkb;
            } else if ( ENABLE_GNOME or ENABLE_KDE or WITH_X ) {
                ERR( "No suitable layouter." );
            } else {
                ERR( "No layouters available." );
            };
        };
        INF( "Selected layouter: " << _settings.layouter );
        if ( _settings.layouter == settings_t::layouter_t::gnome and not is_gnome_desktop() ) {
            WRN(
                _settings.layouter << " layouter selected, "
                    << "but GNOME Shell is not detected, "
                    << "Tapper will likely fail."
            );
        };
        if ( _settings.layouter == settings_t::layouter_t::xkb and not is_x_session() ) {
            WRN(
                _settings.layouter << " layouter selected, "
                    << "but X Window System session is not detected"
            );
        };
        _layouter.reset( layouter_t::create( _settings.layouter ) );
    };
    return * _layouter.get();
};

/**
    Creates (if not yet created) and returns emitter. The method does lazy emitter creation:
    emitter is created when it is actually required.
**/
emitter_t &
app_t::emitter(
) {
    if ( not _emitter ) {
        if ( _settings.emitter <= settings_t::emitter_t::Auto ) {
            if ( _cmdline_parsed and not has_any_key_emits( _settings.assignments ) ) {
                /*
                    If there is no key emittings in assignments, I do not need real emitter and can
                    use the dummy emitter instead. However, `emitter()` can be called during
                    command line parsing, when assignments are not yet fully known. On the other
                    side, if `emitter()` is called at command line parsing, it is called to either
                    check key code or to convert key name to key code. In both cases it means key
                    emitting will be in assignments.
                */
                _settings.emitter = settings_t::emitter_t::dummy;
            } else if ( is_x_session() and WITH_X ) {
                _settings.emitter = settings_t::emitter_t::xtest;
            } else if ( WITH_LIBEVDEV ) {
                _settings.emitter = settings_t::emitter_t::libevdev;
            } else if ( WITH_X or WITH_LIBEVDEV ) {
                ERR( "No suitable emitter." );
            } else {
                ERR( "No emitters available." );
            };
        };
        INF( "Selected emitter: " << _settings.emitter );
        if ( _settings.emitter != settings_t::emitter_t::libevdev ) {
            // Drop "root" user now:
            privileges().drop_root_user();
        };
        if ( _settings.emitter == settings_t::emitter_t::xtest and not is_x_session() ) {
            WRN(
                _settings.emitter << " emitter selected, "
                    << "but X Window System session is not detected, "
                    << "Tapper will likely fail."
            );
        };
        _emitter.reset( emitter_t::create( _settings.emitter ) );
    };
    return * _emitter.get();
};

settings_t::bell_t
app_t::bell(
) {
    if ( _settings.bell == settings_t::bell_t::unset ) {
        _settings.bell = settings_t::bell_t::disabled;
    };
    return _settings.bell;
};

}; // namespace tapper

// end of file //
