#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: configure.ac
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
#   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along with Tapper. If not,
#   see <https://www.gnu.org/licenses/>.
#
#   SPDX-License-Identifier: GPL-3.0-or-later
#
#   ---------------------------------------------------------------------- copyright and license ---

m4_include([m4/v4.m4])
m4_include([m4/autoblank.m4])
m4_include([m4/vc.m4])
m4_include([m4/vs.m4])
m4_include([m4/vm.m4])
m4_include([m4/vdb.m4])

#
#   Init autoconf and automake:
#

VC_META([META.txt])
AC_INIT(VC_PACKAGE_NAME, [VC_PACKAGE_VERSION], [VC_PACKAGE_BUGREPORT], [], [VC_PACKAGE_URL])
AC_CONFIG_SRCDIR([src/tapper.hpp])
AC_CONFIG_MACRO_DIR([_m4])
AC_CONFIG_AUX_DIR([_aux])
VC_CONFIG_BUILDDIR()
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE([])
LT_INIT([disable-shared static])
VC_INIT()

dnl Parameters: User name, project name, sandbox name.
dnl If SF project name is blank, it will be extracted from AC_PACKAGE_URL.
VDB_SF([van-de-bugger], [kbd-tapper], [vdb-sandbox])
VDB_COPR([vandebugger], [tapper], [testing])

GLIB_GSETTINGS()

#
#   Command-line options:
#

VC_ARG_ENABLE([ emitters ],[ yes no ],[
    enable/disable emitters
])
VC_ARG_ENABLE([ layouters ],[ yes no ],[
    enable/disable layouters
])
VC_ARG_ENABLE([ gnome ],[ yes no ],[
    enable/disable GNOME support
])
VC_ARG_ENABLE([ kde ],[ yes no ],[
    enable/disable KDE support
])
VC_ARG_ENABLE([ debug ],[ no yes ],[
    enable/disable debug build
])
VC_ARG_ENABLE([ man ],[ auto yes no ],[
    enable/disable building man pages;
    auto == enable man if pandoc program found
])
VC_ARG_ENABLE([ metainfo ],[ auto yes no ],[
    enable/disable building metainfo.xml file;
    auto == enable man if pandoc program found
])
VC_ARG_ENABLE([ rpm ],[ auto yes no ],[
    enable/disable building and testing rpm packages;
    auto == enable rpm if rpmbuild program found
])
VC_ARG_ENABLE([ html ],[ auto yes no ],[
    enable building and testing html pages;
    auto == enable html if pandoc program found
])
VC_ARG_ENABLE([ author-testing ],[ auto yes no ],[
    enable author testing;
    auto == enable if AUTHOR_TESTING is set
])

VC_ARG_WITH([ libcap ],[ yes no ],[
    use libcap
])
VC_ARG_WITH([ glib ],[ yes no ],[
    use GLib(mm) and GIO(mm) libraries
])
VC_ARG_WITH([ libevdev ],[ yes no ],[
    use libevdev library
])
VC_ARG_WITH([ libinput ],[ yes no ],[
    use libinput library
])
VC_ARG_WITH([ x ],[ yes no ],[
    use X Window System libraries
])

BS_IF([
    test x"$enable_emitters$with_libevdev" = x"yesno"
],[
    VC_MSG_ERROR([ Emitters require libevdev library ])
])
BS_IF([
    test x"$enable_layouters$with_libinput$with_x" = x"yesnono"
],[
    VC_MSG_ERROR([ Layouters require either libinput library and/or Window System libraries ])
])

BS_IF([
    test x"$enable_gnome$with_glib" = x"yesno"
],[
    VC_MSG_ERROR([ GNOME support requires GLib libraries ])
])
BS_IF([
    test x"$enable_kde$with_glib" = x"yesno"
],[
    VC_MSG_ERROR([ KDE support requires GLib libraries ])
])

BS_IF([
    test x"$enable_shared$enable_static" = x"yesyes"
],[
    VC_MSG_NOTICE([
        having both shared and static modules is pointless => static modules are disabled
    ])
    BS_VAR_SET([ enable_static ],[ no ])
])

BS_CASE([
    $enable_author_testing
],[ auto ],[
    # Do nothing.
],[
    yes
],[
    BS_VAR_SET([ AUTHOR_TESTING ],[ 1 ])
],[
    no
],[
    BS_VAR_SET([ AUTHOR_TESTING ],[])
])

#
#   Check programs:
#

AC_PROG_CXX()
VC_CHECK_PROG([ PERL ],[ perl interpreter ],[ perl ],[],[
    VC_MSG_ERROR([ perl not found ])
])
VC_CHECK_PROG([ TAR ],[ tar archiver ],[ tar ],[],[
    VC_MSG_ERROR([ tar not found ])
])
BS_IF([
    test x"${AUTHOR_TESTING+yes}" = x"yes"
],[
    VC_CHECK_PROG([ CPPCHECK ],[ cppcheck program ],[ cppcheck ],[],[
        VC_MSG_WARN([ cppcheck not found => some tests will be skipped ])
    ])
])

VS_ARG_IF_WITH([
    libcap
],[
    # On Debian, `sbin` directories are not in the PATH, while `setcap` program resides in
    # `/usr/sbin` directory. Slackware still uses `/sbin` directory.
    BS_VAR_SET([ _path ],[ $PATH ])
    BS_VAR_SET([ PATH ],[ $PATH:/usr/local/sbin:/usr/sbin:/sbin ])
    VC_CHECK_PROG([ SETCAP ],[ setcap program ],[ setcap ],[],[
        VC_MSG_ERROR([ setcap not found ])
    ])
    BS_VAR_SET([ PATH ],[ $_path ])
])
VS_ARG_IF_WITH([
    libinput
],[
    VC_CHECK_PROG([ CHGRP ],[ chgrp program ],[ chgrp ],[],[
        VC_MSG_ERROR([ chgrp not found ])
    ])
    VC_CHECK_PROG([ CHMOD ],[ chmod program ],[ chmod ],[],[
        VC_MSG_ERROR([ chmod not found ])
    ])
])
VC_CHECK_PROG([ DESKTOP_FILE_VALIDATE ],[ desktop-file-validate program ],[ desktop-file-validate ],[],[
    VC_MSG_WARN([ desktop-file-validate not found => some tests will be skipped ])
])
VC_CHECK_PROG([ APPSTREAM ],[ appstreamcli program ],[ appstreamcli ],[],[
    VC_MSG_WARN([ appstreamcli not found => some tests will be skipped ])
])

VC_CHECK_PROG_AUTO([ man ],[ PANDOC ],[ pandoc program ],[ pandoc ])
VS_ARG_IF_ENABLED([
    man
],[
    VC_CHECK_PROG([ M4 ],[ m4 program ],[ m4 ],[],[
        VC_MSG_ERROR([ m4 not found ])
    ])
    VC_CHECK_PROG([ MAN ],[ man program ],[ man ],[],[
        VC_MSG_WARN([ man not found => some tests will be skipped ])
    ])
    VC_CHECK_PROG([ ASPELL ],[ aspell program ],[ aspell ],[],[
        VC_MSG_WARN([ aspell not found => some tests will be skipped ])
    ])
])

VC_CHECK_PROG_AUTO([ metainfo ],[ PANDOC ],[ pandoc program ],[ pandoc ])

VC_CHECK_PROG([ RSYNC ],[ rsync program ],[ rsync ],[
    BC_SUBST([ UPLOAD ],[ "$RSYNC -av" ])
], [
    VC_CHECK_PROG([ SCP ],[ scp program ],[ scp ],[
        BC_SUBST([ UPLOAD ],[ "$SCP -p" ])
        VC_MSG_WARN([ rsync not found => uploading with scp may leave obsolete files ])
    ],[
        VC_MSG_WARN([ neither rsync nor scp found => upload targets are disabled ])
    ])
])

# Rpm build and check tools:
VC_CHECK_PROG_AUTO([ rpm ],[ RPMBUILD ],[ rpmbuild program ],[ rpmbuild ])
VS_ARG_IF_ENABLED([
    rpm
],[
    VC_CHECK_RPM_DIST()
    VC_CHECK_RPM_ARCH()
    VC_CHECK_PROG([ RPMLINT ],[ rpmlint program ],[ rpmlint ],[],[
        VC_MSG_WARN([ rpmlint not found => some tests will be skipped ])
    ])
    VC_CHECK_PROG([ COPR ],[ copr program ],[ copr-cli ])
])

# HTML build and check tools:
BS_CASE([
    $enable_html
],[
    auto
],[
    VC_CHECK_PROG([ PANDOC ],[ pandoc program ],[ pandoc ],[],[
        VC_MSG_WARN([ --enable-html=auto[,] pandoc not found => html disabled ])
        BS_VAR_SET([ enable_html ],[ "no" ])
    ])
    VC_CHECK_PROG([ WGET ],[ wget program ],[ wget ],[],[
        VC_MSG_WARN([ --enable-html=auto[,] wget not found => html disabled ])
        BS_VAR_SET([ enable_html ],[ "no" ])
    ])
    BS_CASE([ $enable_html ],[ auto ],[
        VC_MSG_RESULT([ --enable-html=auto, pandoc and wget found => html enabled ])
        BS_VAR_SET([ enable_html ],[ "yes" ])
    ])
    BS_IF([
        test x"$enable_html" = x"yes" -a x"${AUTHOR_TESTING+yes}" = x"yes"
    ],[
        VC_CHECK_PROG([ TIDY ],[ tidy program ],[ tidy ],[],[
            VC_MSG_WARN([ tidy not found => some tests will be skipped ])
        ])
    ])
    VM_CONDITIONAL([ enable_html ])
],[
    yes
],[
    VC_CHECK_PROG([ PANDOC ],[ pandoc program ],[ pandoc ],[],[
        VC_MSG_ERROR([ pandoc not found ])
    ])
    VC_CHECK_PROG([ WGET ],[ wget program ],[ wget ],[],[
        VC_MSG_ERROR([ wget not found ])
    ])
    VC_CHECK_PROG([ M4 ],[ m4 program ],[ m4 ],[],[
        VC_MSG_ERROR([ m4 not found ])
    ])
    BS_IF([
        test x"${AUTHOR_TESTING+yes}" = x"yes"
    ],[
        VC_CHECK_PROG([ TIDY ],[ tidy program ],[ tidy ],[],[
            VC_MSG_WARN([ tidy not found => some tests will be skipped ])
        ])
    ])
])

VC_CHECK_PROG([ DOXYGEN ],[ doxygen program ],[ doxygen ],[],[
    VC_MSG_WARN([ doxygen not found => dox target is disabled ])
])

#
#    Check libraries:
#

AC_LANG(C++)

BS_IF([
    test x"$with_libcap" = x"yes"
],[
    PKG_CHECK_MODULES([LIBCAP], [libcap])
])
BS_IF([
    test x"$with_glib" = x"yes"
],[
    PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4])
    PKG_CHECK_MODULES([GIOMM], [giomm-2.4])
])
BS_IF([
    test x"$with_libevdev" = x"yes"
],[
    PKG_CHECK_MODULES([LIBEVDEV], [libevdev])
])
BS_IF([
    test x"$with_libinput" = x"yes"
],[
    PKG_CHECK_MODULES([UDEV], [libudev])
    PKG_CHECK_MODULES([LIBINPUT], [libinput])
])
BS_IF([
    test x"$with_x" = x"yes"
],[
    PKG_CHECK_MODULES([X11], [x11])
    PKG_CHECK_MODULES([XTST], [xtst])
])

BC_DEFINE([ _REENTRANT ],[ 1 ])
    # Otherwise AX_PTHREAD will fail to detect working pthread with no options.
AX_PTHREAD([
    # ok.
],[
    VC_MSG_ERROR([ No working pthread found. ])
])

#
#   Check headers:
#

BS_IF([
    test x"$with_libinput" = x"yes"
],[
    BC_CHECK_HEADER([
        linux/input-event-codes.h
    ],[
        BS_VAR_SET([HAVE_LINUX_INPUT_EVENT_CODES_H],[yes])
    ],[
        BS_VAR_SET([HAVE_LINUX_INPUT_EVENT_CODES_H],[no])
    ])
])

#
#   Define macros:
#

BS_IF([
    test x"$enable_debug" = x"yes"
],[
    BC_DEFINE([ ENABLE_DEBUG ],[ 1 ])
],[
    BC_DEFINE([ ENABLE_DEBUG ],[ 0 ])
])
BS_IF([
    test x"$enable_emitters" = x"yes"
],[
    BC_DEFINE([ ENABLE_EMITTERS ],[ 1 ])
],[
    BC_DEFINE([ ENABLE_EMITTERS ],[ 0 ])
])
BS_IF([
    test x"$enable_layouters" = x"yes"
],[
    BC_DEFINE([ ENABLE_LAYOUTERS ],[ 1 ])
],[
    BC_DEFINE([ ENABLE_LAYOUTERS ],[ 0 ])
])
BS_IF([
    test x"$enable_gnome" = x"yes"
],[
    BC_DEFINE([ ENABLE_GNOME ],[ 1 ])
],[
    BC_DEFINE([ ENABLE_GNOME ],[ 0 ])
])
BS_IF([
    test x"$enable_kde" = x"yes"
],[
    BC_DEFINE([ ENABLE_KDE ],[ 1 ])
],[
    BC_DEFINE([ ENABLE_KDE ],[ 0 ])
])
BS_IF([
    test x"$with_glib" = x"yes"
],[
    BC_DEFINE([ WITH_GLIB ],[ 1 ])
],[
    BC_DEFINE([ WITH_GLIB ],[ 0 ])
])
BS_IF([
    test x"$with_libcap" = x"yes"
],[
    BC_DEFINE([ WITH_LIBCAP ],[ 1 ])
],[
    BC_DEFINE([ WITH_LIBCAP ],[ 0 ])
])
BS_IF([
    test x"$with_libevdev" = x"yes"
],[
    BC_DEFINE([ WITH_LIBEVDEV ],[ 1 ])
],[
    BC_DEFINE([ WITH_LIBEVDEV ],[ 0 ])
])
BS_IF([
    test x"$with_libinput" = x"yes"
],[
    BC_DEFINE([ WITH_LIBINPUT ],[ 1 ])
],[
    BC_DEFINE([ WITH_LIBINPUT ],[ 0 ])
])
BS_IF([
    test x"$with_x" = x"yes"
],[
    BC_DEFINE([ WITH_X ],[ 1 ])
],[
    BC_DEFINE([ WITH_X ],[ 0 ])
])
BS_IF([
    test x"$HAVE_LINUX_INPUT_EVENT_CODES_H" = x"yes"
],[
    BC_DEFINE([ HAVE_LINUX_INPUT_EVENT_CODES_H ],[ 1 ])
],[
    BC_DEFINE([ HAVE_LINUX_INPUT_EVENT_CODES_H ],[ 0 ])
])

#
#   Output files:
#

AH_TEMPLATE([_REENTRANT], [Do not touch this.])
AH_TEMPLATE([ENABLE_DEBUG], [Define to enable debug features.])
AH_TEMPLATE([ENABLE_EMITTERS], [Define to enable emitters.])
AH_TEMPLATE([ENABLE_LAYOUTERS], [Define to enable layouters.])
AH_TEMPLATE([ENABLE_GNOME], [Define to enable GNOME support.])
AH_TEMPLATE([ENABLE_KDE], [Define to enable KDE support.])
AH_TEMPLATE([HAVE_LINUX_INPUT_EVENT_CODES_H], [Have <linux/input-event-codes.h>?])
AH_TEMPLATE([WITH_GLIB], [Define to use glib library.])
AH_TEMPLATE([WITH_LIBCAP], [Define to use libcap library.])
AH_TEMPLATE([WITH_LIBEVDEV], [Define to use libevdev library.])
AH_TEMPLATE([WITH_LIBINPUT], [Define to use libinput library.])
AH_TEMPLATE([WITH_X], [Define to use X Window System libraries.])

VM_CONDITIONAL([ enable_shared enable_static ])
VM_CONDITIONAL([ AUTHOR_TESTING ])
VM_CONDITIONAL([ COPR RSYNC SCP UPLOAD ])
VM_CONDITIONAL([ HAVE_LINUX_INPUT_EVENT_CODES_H ])

BC_CONFIG_HEADERS([ config.h ])
BC_CONFIG_FILES([ Makefile ])
BC_OUTPUT()

# Show summary of warnings and notices:
VC_MSG_SUMMARY()

# end of file #
