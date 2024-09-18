/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/xdg.cpp

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
    TODO
**/

#include "xdg.hpp"

#include "posix.hpp"
#include "string.hpp"
#include "test.hpp"

namespace tapper {
namespace xdg {

static char const * const XDG_CONFIG_HOME = "XDG_CONFIG_HOME";
static char const * const XDG_CONFIG_DIRS = "XDG_CONFIG_DIRS";

/*
    Returns base directory for user configuration.
*/
string_t
config_home(
) {
    return posix::get_env( XDG_CONFIG_HOME, posix::get_home() + "/.config" );
};

TEST(
    char const * const home_backup = getenv( XDG_CONFIG_HOME );

    ASSERT_EQ( unsetenv( XDG_CONFIG_HOME ), 0 );
    ASSERT_EQ( config_home(), posix::get_home() + "/.config" );

    ASSERT_EQ( setenv( XDG_CONFIG_HOME, "", 1 ), 0 );
    ASSERT_EQ( config_home(), posix::get_home() + "/.config" );

    ASSERT_EQ( setenv( XDG_CONFIG_HOME, "/home/user/.config", 1 ), 0 );
    ASSERT_EQ( config_home(), "/home/user/.config" );

    if ( home_backup ) {
        ASSERT_EQ( setenv( XDG_CONFIG_HOME, home_backup, 1 ), 0 );
    } else {
        ASSERT_EQ( unsetenv( XDG_CONFIG_HOME ), 0 );
    };
);

/*
    Returns list of directories to search for configurations files. By default, the user config
    directory is included to the beginning of the list.
*/
strings_t
config_dirs(
    bool include_home   ///< If `false` the user configuration directory will be excluded.
) {
    auto const dirs = split( ':', posix::get_env( XDG_CONFIG_DIRS, "/etc/xdg" ) );
    return include_home ? concat( xdg::config_home(), dirs ) : dirs;
};

TEST(
    char const * const dirs_backup = getenv( XDG_CONFIG_DIRS );

    ASSERT_EQ( unsetenv( XDG_CONFIG_DIRS ), 0 );
    ASSERT_EQ( config_dirs( false ), strings_t( { "/etc/xdg" } ) );
    ASSERT_EQ( config_dirs( true  ), strings_t( { config_home(), "/etc/xdg" } ) );
    ASSERT_EQ( config_dirs(),        strings_t( { config_home(), "/etc/xdg" } ) );

    ASSERT_EQ( setenv( XDG_CONFIG_DIRS, "", 1 ), 0 );
    ASSERT_EQ( config_dirs( false ), strings_t( { "/etc/xdg" } ) );
    ASSERT_EQ( config_dirs( true  ), strings_t( { config_home(), "/etc/xdg" } ) );
    ASSERT_EQ( config_dirs(       ), strings_t( { config_home(), "/etc/xdg" } ) );

    ASSERT_EQ( setenv( XDG_CONFIG_DIRS, "/a:/b:/c", 1 ), 0 );
    ASSERT_EQ( config_dirs( false ), strings_t( { "/a", "/b", "/c" } ) );
    ASSERT_EQ( config_dirs( true  ), strings_t( { config_home(), "/a", "/b", "/c" } ) );
    ASSERT_EQ( config_dirs(       ), strings_t( { config_home(), "/a", "/b", "/c" } ) );

    if ( dirs_backup ) {
        ASSERT_EQ( setenv( XDG_CONFIG_DIRS, dirs_backup, 1 ), 0 );
    } else {
        ASSERT_EQ( unsetenv( XDG_CONFIG_DIRS ), 0 );
    };
);

}; // namespace xdg
}; // namespace tapper

// end of file //
