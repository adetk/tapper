#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: m4/vdb.m4
#
#   Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
#
#   This file is part of Tapper.
#
#   Tapper is free software: you can redistribute it and/or modify it under the terms of the GNU
#   General Public License as published by the Free Software Foundation, either version 3 of the
#   License, or (at your option) any later version.
#
#   Tapper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
#   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along with Tapper.  If not,
#   see <http://www.gnu.org/licenses/>.
#
#   SPDX-License-Identifier: GPL-3.0-or-later
#
#   ---------------------------------------------------------------------- copyright and license ---

#
#   Autoconf macros which are too author-specific to be useful outside the project.
#

#   vdb_if_trial_release(IF-TRIAL, IF-NOT)
#   ------------------------------------------------------------------------------------------------
m4_define([vdb_if_trial_release],
    [m4_cond(
        [VC_PACKAGE_TRIAL],
        [0],
        [$2],
        [$1])])


#   VDB_SF(USER, PROJ, SANDBOX)
#   ------------------------------------------------------------------------------------------------
m4_define([VDB_SF], [
    m4_pushdef([user], [[$1]])
    m4_ifblank(user, [m4_fatal([$0: user name is not specified])])
    m4_pushdef([proj])
    vdb_if_trial_release([
        m4_define([proj], [[$3]])
        m4_ifblank(proj, [m4_fatal([$0: test project name is not specified])])
    ],[
        m4_define([proj], m4_bregexp(AC_PACKAGE_URL, [^https?://\(.*\)\.sourceforge\.\(net\|io\)/?$], [[[\1]]]))
        m4_ifblank(proj, [m4_fatal([$0: cannot extract sf project name from package url: ]AC_PACKAGE_URL)])
    ])
    m4_pushdef([fixed],  [https://sourceforge.net/projects/proj])
    m4_pushdef([custom], [https://sourceforge.net/p/proj])
    # SourceForge user-visible pages:
    AC_SUBST([PACKAGE_SF_WEBSITE],      ['fixed/'])
    AC_SUBST([PACKAGE_SF_SUMMARY],      ['fixed/'])
    AC_SUBST([PACKAGE_SF_FILES],        ['fixed/files/AC_PACKAGE_VERSION/'])
    AC_SUBST([PACKAGE_SF_REVIEWS],      ['fixed/reviews/'])
    AC_SUBST([PACKAGE_SF_TICKETS],      ['custom/tickets/'])
    AC_SUBST([PACKAGE_SF_FORUMS],       ['custom/forums/'])
    # SourceForge upload URLs:
    AC_SUBST([PACKAGE_SF_UPLOAD_HTML],  ['user,proj@web.sourceforge.net:htdocs/'])
    AC_SUBST([PACKAGE_SF_UPLOAD_FILES], ['user,proj@frs.sourceforge.net:/home/frs/project/v4_left(proj, 1)/v4_left(proj, 2)/proj/AC_PACKAGE_VERSION/'])
    m4_popdef([fixed])
    m4_popdef([custom])
    m4_popdef([proj])
    m4_popdef([user])
])


#   VDB_COPR(USER, PROJ, SANDBOX)
#   ------------------------------------------------------------------------------------------------
m4_define([VDB_COPR], [
    m4_pushdef([user], [[$1]])
    m4_ifblank(user, [m4_fatal([$0: user name is not specified])])
    m4_pushdef([proj])
    vdb_if_trial_release([
        m4_define([proj], [[$3]])
        m4_ifblank(proj, [m4_fatal([$0: test project name is not specified])])
    ],[
        m4_define([proj], [[$2]])
        m4_ifblank(proj, [m4_fatal([$0: project name is not specified])])
    ])
    dnl Values to pass down to makefile:
    BC_SUBST([ PACKAGE_COPR_PRJ  ],[ 'user/proj' ])
    BC_SUBST([ PACKAGE_COPR_REPO ],[ 'https://copr.fedorainfracloud.org/coprs/user/proj/' ])
    BC_SUBST([ PACKAGE_COPR_PKG  ],[ 'https://copr.fedorainfracloud.org/coprs/user/proj/package/AC_PACKAGE_TARNAME' ])
    m4_popdef([proj])
    m4_popdef([user])
])


# end of file #
