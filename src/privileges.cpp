/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/privileges.cpp

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
    Privilege manager implementation.
**/

#include "privileges.hpp"

#include <sys/prctl.h>

#include "string.hpp"
#include "posix.hpp"

namespace tapper {

privileges_t &
privileges(
) {
    static privileges_t privileges;
    return privileges;
};

void
privileges_t::show(
) {
    #if ENABLE_DEBUG
        lock_t lock( _mutex );
        auto uids = posix::get_user_ids();
        auto gids = posix::get_group_ids();
        auto groups = posix::get_groups();
        INF( "User ids: " << uids << "." );
        INF( "Group ids: " << gids << "." );
        INF( "Supplementary groups: " << groups << "." );
        #if WITH_LIBCAP
            INF( "Capabilities: " << posix::cap_t() << "." );
        #endif // WITH_LIBCAP
        assert( uids == _uids );    // Make sure I maintain _uids correctly.
        assert( gids == _gids );    // Make sure I maintain _gids correctly.
    #endif // ENABLE_DEBUG
};

void
privileges_t::init(
) {

    lock_t lock( _mutex );

    {
        auto err = prctl( PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0 );
        if ( err ) {
            using posix::error_t;
            int e = errno;
            ERR( "Can't set no_new_privs bit", e );
        };
    }

    #if WITH_LIBCAP

        using flag_t  = posix::cap_t::flag_t;
        using value_t = posix::cap_t::value_t;

        posix::cap_t cap;

        // Drop the supplementary groups.
        if ( cap.get( flag_t::permitted, value_t::setgid ) ) {
            if ( not cap.get( flag_t::effective, value_t::setgid ) ) {
                cap.set( flag_t::effective, value_t::setgid, true );
                cap.apply();
            };
            posix::set_groups();   // Clear supplementary groups, just in case.
                /*
                    Supplementary group list can contain "input" group, throwing it away doesn't
                    seem like a good idea. However, the process has `cap_setgid` capability, so I
                    can re-introduce "input" as saved gid anyway.
                */
            set_group_ids( _gids.r, WITH_LIBINPUT ? _input() : _gids.r );
        } else {
            /*
                The process does not have privileges to change gids arbitrary. All what I can is
                shuffling real, effective, and saved gids. In case of `chgrp input && chmod g+s`
                both effective and saved gids are "input", so I can safely reset the effective gid
                now (and restore it to "input" later, in `do_as_input`). However, if Tapper
                compiled without libinput, I will no need "input" group and can reset saved group
                too.
            */
            set_group_ids( _gids.r, WITH_LIBINPUT ? no_gid : _gids.r );
        };

        if ( cap.get( flag_t::permitted, value_t::setuid ) ) {
            if ( not cap.get( flag_t::effective, value_t::setuid ) ) {
                cap.set( flag_t::effective, value_t::setuid, true );
                cap.apply();
            };
            set_user_ids( _uids.r, WITH_LIBEVDEV ? _root() : _uids.r );
        } else {
            /*
                The process does not have privileges to change uids arbitrary. All what I can is
                shuffling real, effective, and saved uids. In case of `chown root && chmod u+s`
                both effective and saved gids will be "root", so I can safely reset the effective
                gid now (and restore it to "root" later, in `do_as_root`).
            */
            set_user_ids( _uids.r, WITH_LIBEVDEV ? no_uid : _uids.r );
        };

        // Drop all capabilities now, they are not required any more:
        cap.clear();
        cap.apply();

    #else

        /*
            If Tapper is built without libcap, I do not know if the process have required
            privileges or not. Checking for `uids.e == _root` does not help because even the `root`
            user may have incomplete set of permitted capabilities or no capabilities at all. All I
            can is just try to perform required actions and ignore errors.

            Errors should be ignored because at this moment It is not yet known if the "input"
            group and "root" user will be actually used — it depends on settings and command line
            options. (For example, neither XRecord listener nor XTest emitter require extra
            permissions.)

            Change groups before changing the user, otherwise changing groups may fail due to lack
            of privileges.
        */
        CATCH_ALL( set_group_ids( _gids.r, WITH_LIBINPUT ? input : _gids.r ) );
        CATCH_ALL( set_user_ids( _uids.r, WITH_LIBEVDEV ? root : _uids.r ) );

    #endif // WITH_LIBCAP
};

void
privileges_t::drop_input_group(
) {
    lock_t lock( _mutex );
    TRACE();
    set_group_ids( _gids.r, _gids.r );
};

void
privileges_t::drop_root_user(
) {
    lock_t lock( _mutex );
    TRACE();
    set_user_ids( _uids.r, _uids.r );
};

void
privileges_t::do_as_input(
    function_t function
) {
    lock_t lock( _mutex );
    TRACE();
    bool set = _gids.e != _gids.s;
    /*
        If saved gid is not "input", the libinput listener will likely fail. However, I do not know
        it for sure: the user's supplementary group list may include "input" group, or files in
        `/dev/input` directory are readable by the user due to another reason.
    */
    if ( set ) {
        set_group_ids( _gids.s );
    };
    function();
    if ( set ) {
        set_group_ids( _gids.r );
    };
};

void
privileges_t::do_as_root(
    function_t function
) {
    lock_t lock( _mutex );
    bool set = _uids.e != _uids.s;
    /*
        If saved uid is not "root", the libevdev listener will likely fail. However, I do not know
        it for sure.
    */
    if ( set ) {
        set_user_ids( _uids.s );
    };
    function();
    if ( set ) {
        set_user_ids( _uids.r, _uids.r );
    };
};

void
privileges_t::do_as_user(
    function_t function
) {
    lock_t lock( _mutex );
    TRACE();
    function();
};

uid_t
privileges_t::_root(
) {
    return 0;
};

gid_t
privileges_t::_input(
) {
    static gid_t input { no_gid };
    if ( input == no_gid ) {
        input = posix::get_group_id( "input" );
    };
    return input;
};

privileges_t::privileges_t(
):
    OBJECT_T(),
    _uids( posix::get_user_ids() ),
    _gids( posix::get_group_ids() )
{
    show();
};

void
privileges_t::set_user_ids(
    uid_t effective,
    uid_t saved
) {
    if ( _uids.e != effective or _uids.s != saved ) {
        posix::set_user_ids( { no_uid, effective, saved } );
        if ( effective != no_uid ) {
            _uids.e = effective;
        };
        if ( saved != no_uid ) {
            _uids.s = saved;
        };
    };
};

void
privileges_t::set_group_ids(
    gid_t effective,
    gid_t saved
) {
    if ( _gids.e != effective or _gids.s != saved ) {
        posix::set_group_ids( { no_gid, effective, saved } );
        if ( effective != no_gid ) {
            _gids.e = effective;
        };
        if ( saved != no_gid ) {
            _gids.s = saved;
        };
    };
};

}; // namespace tapper

// end of file //
