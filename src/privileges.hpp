/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/privileges.hpp

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
    Privilege manager interface.

    Stuff is a bit complicated:

    *   libinput listener requires that process belongs to "input" group (or to "root" user) in
        order to open files in "/dev/input/" directory. Since new files may appear in the directory
        at run time (e. g. if user plugs in an USB keybord), and libinput will open these new
        files, Tapper should keep "input" group as long as libinput listener is in use.

    *   libevdev emitter requires that process belongs to "root" user in order to open
        "/dev/uinput" file. As soon as the file open, "root" user can be dropped permanently.

    *   XRecord listener and Xkb layouter do not require any extra permissions and perfectly works
        from regular user. However, connection to display fails if process belongs to "root" user.

    *   The same for GNOME and KDE layouters: D-Bus connection fails in case of "root" user.

    So, `chown root:input tapper && chmod g+s` is enough for libinput listener, but not enough for
    libevdev emitter — "input" group can't write "/dev/uinput". `chown root:root && chmod g+s` is
    enough for libevdev emitter, but is not enough for libinput listener — "root" group can't read
    "/dev/input/". `chown root:root && chmod u+s` is enough for both libinput listener and libevdev
    emitter, but breaks X, GNOME and KDE layouters. To satisfy all components, user and group
    should be changed in run time.

    Ok, let's start from `chown root:root && chmod u+s` and drop extra permissions as soon as
    possible:

    1.  At the very beginning: drop all supplementary groups; set effective user to real user,
        set saved user to "root" so Tapper can switch back to "root" user for further management;
        set saved group to "input", set effective group to real group.

    2.  If libinput listener is not used: drop "input" group (set both effective and saved group to
        real group).

    3.  If libevdev emitter is not used: drop "root" user completely (set both effective user and
        saved user to real user).

    4.  If libinput listener used, set effective group to saved group (= "input") temporarily, only
        for open file in "/dev/input".

    5.  If libevdev emitter used, to let it open "/dev/uinput": switch back to "root" user, open
        the file, then drop "root" user completely (set both effective user and saved user to real
        user).

    6.  Use a mutex to make sure GNOME and KDE layouter does not open D-Bus connection when Tapper
        temporary switched back to "root" user (step 4). Of course, the same mutex should be used
        in other steps.

    P. S. Actually, `chown root:root && chmod u+s` is to much. Only two capabilities, `cap_setuid`
    and `cap_setgid` are enough.

    @sa privileges.cpp
**/

#ifndef _TAPPER_PRIVILEGES_HPP_
#define _TAPPER_PRIVILEGES_HPP_

#include "base.hpp"

#include "posix.hpp"

#include <functional>
#include <mutex>

namespace tapper {

    /**
        Privileges management singleton. This is not general-purpose class, it is sutable only for
        Tapper and implements plan described above.
    **/
    class privileges_t: public object_t {

        friend privileges_t & privileges();

        public:

            using mutex_t    = std::mutex;
            using lock_t     = std::lock_guard< mutex_t >;
            using function_t = std::function< void() >;

            void show();

            /**
                Drop all supplementary groups, set saved group to "input", effective group to real
                group, set saved user to "root", set effective user to real user (do step 1 of the
                plan).
            **/
            void init();

            /** Drop "input" group (step 2). **/
            void drop_input_group();

            /**
                Drop "root" user completely, it will not possible to switch back to "root" user
                after dropping (step 3).
            **/
            void drop_root_user();

            /**
                Execute the given function as "input" group (step 4).
            **/
            void do_as_input( function_t function );

            /**
                Execute function as "root" user and then drop "root" user (step 5).
            **/
            void do_as_root( function_t function );

            /**
                Execute the given function and make sure the other thread doesn't run something as
                "root" user (step 6).
            **/
            void do_as_user( function_t function );

        private:

            static auto constexpr no_uid = posix::no_uid;
            static auto constexpr no_gid = posix::no_gid;

            static uid_t _root();       /**< "root" user `uid_t`. */
            static gid_t _input();      /**< "input" group `gid_t`. */

            /** Private constructor to prevent creating more than one instance. **/
            privileges_t();

            void  set_user_ids( uid_t effective, uid_t saved = no_uid );
            void  set_group_ids( gid_t effective, gid_t saved = no_gid );

            std::mutex  _mutex;

            posix::res_uids_t   _uids;      ///< uids: real, effective, saved.
            posix::res_gids_t   _gids;      ///< gids: real, effective, saved.

    }; // class privileges_t

    privileges_t & privileges();

}; // namespace tapper

#endif // _TAPPER_PRIVILEGES_HPP_

// end of file //
