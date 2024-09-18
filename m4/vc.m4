#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: m4/vc.m4
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
#   see <https://www.gnu.org/licenses/>.
#
#   SPDX-License-Identifier: GPL-3.0-or-later
#
#   ---------------------------------------------------------------------- copyright and license ---


#
#   Autoconf macro extensions.
#


#   VC_ARG_ENABLE( FEATURE, VALUES, HELP )
#   ------------------------------------------------------------------------------------------------
#
BC_DEFUN([
    VC_ARG_ENABLE
],[
    m4_pushdef([option], v4_trim([ $1 ]))
    m4_pushdef([feature], v4_trim(m4_bpatsubst([ $1 ],[-],[_])))
    m4_pushdef([values], m4_split(m4_normalize([ $2 ])))
    m4_pushdef([default], m4_argn(1, values))
    VS_VAR_PUSHDEF([ variable ],[ [enable_]feature ])
    m4_divert_text([DEFAULTS], [variable[="]default["]])
    BC_ARG_ENABLE([
        feature
    ],[
        BS_HELP_STRING([
            [--enable-]option[=]m4_join([|],values)
        ],[
            ]m4_normalize([ $3 (default: ]m4_argn(1, values)[) ])[
        ])
    ],[
        # Action if option given: check value.
        BS_CASE([
            "$enableval"
        ],[
            m4_join([|], values)
        ],[
            BS_VAR_SET([
                variable
            ],[
                "$enableval"
            ])
        ],[
            VC_MSG_ERROR([
                [bad value of --enable-]option[ option: $enableval;]
                [must be one of: ]values
            ])
        ])
    ])
    VM_CONDITIONAL([ enable_]feature[ ])
    VS_VAR_POPDEF([ variable ])
    m4_popdef([option])
    m4_popdef([feature])
    m4_popdef([values])
    m4_popdef([default])
])


#   VC_ARG_WITH( PACKAGE, VALUES, HELP )
#   ------------------------------------------------------------------------------------------------
#
BC_DEFUN([
    VC_ARG_WITH
],[
    m4_pushdef([package], v4_trim([ $1 ]))
    m4_pushdef([values], m4_split(m4_normalize([ $2 ])))
    m4_pushdef([default], m4_argn(1, values))
    VS_VAR_PUSHDEF([ variable ],[ [with_]package ])
    m4_divert_text([DEFAULTS], [variable[="]default["]])
    BC_ARG_WITH([
        package
    ],[
        BS_HELP_STRING([
            [--with-]package[=]m4_join([|],values)
        ],[
            ]m4_normalize([ $3 (default: ]m4_argn(1, values)[) ])[
        ])
    ],[
        # Action if option given: check value.
        BS_CASE([
            "$withval"
        ],[
            m4_join([|], values)
        ],[
            BS_VAR_SET([
                variable
            ],[
                "$withval"
            ])
        ],[
            VC_MSG_ERROR([
                [bad value of --with-]package[ option: $witheval;]
                [must be one of: ]values
            ])
        ])
    ])
    VM_CONDITIONAL([ with_]package[ ])
    VS_VAR_POPDEF([ variable ])
    m4_popdef([package])
    m4_popdef([values])
    m4_popdef([default])
])


#   _VC_MSG( LEVEL, MESSAGE )
#   ------------------------------------------------------------------------------------------------
#   Do not call it directly.
#   Internal guts of VC_NOTICE and VC_WARN. LEVEL is the message level, NOTICE or WARN.
#
BC_DEFUN([
    _VC_MSG
],[
    m4_pushdef([level], v4_trim([ $1 ]))
    m4_pushdef([message], m4_normalize([ $2 ]))
    m4_indir([AC_MSG_]level, message)
    VS_VAR_PUSHDEF([ log ],[ [_VC_]level[_LOG] ])
    BS_VAR_APPEND([ log ],[ "m4_n(message.)" ])
    VS_VAR_POPDEF([ log ])
    m4_popdef([level])
    m4_popdef([message])
])


#   VC_MSG_NOTICE( MESSAGE )
#   ------------------------------------------------------------------------------------------------
#   Issue a normalized message by AC_MSG_NOTICE and append it to the notice log.
#
BC_DEFUN([
    VC_MSG_NOTICE
],[
    _VC_MSG([ NOTICE ],[ $1 ])
])


#   VC_MSG_WARN( MESSAGE )
#   ------------------------------------------------------------------------------------------------
#   Issue a normalized message by AC_MSG_WARN and append it to the warn log.
#
BC_DEFUN([
    VC_MSG_WARN
],[
    _VC_MSG([ WARN ],[ $1 ])
])


#   VC_MSG_ERROR( MESSAGE )
#   ------------------------------------------------------------------------------------------------
#   Issue a normalized message by AC_MSG_ERROR. Since AC_MSG_ERROR stops configure, there is no
#   need in appending the message to the log.
#
BC_DEFUN([
    VC_MSG_ERROR
],[
    BC_MSG_ERROR( m4_normalize([ $1 ]) )
])


#   VC_MSG_RESULT( MESSAGE )
#   ------------------------------------------------------------------------------------------------
#   Same as AC_MSG_RESULT but accepts name of shell variable and prints (empty) if variable value
#   is empty string.
#
BC_DEFUN([
    VC_MSG_RESULT
],[
    BS_VAR_SET([ $0_str ], "m4_normalize([ $1 ])" )
    VS_VAR_IF_EMPTY([
        $0_str
    ],[
        BS_VAR_SET([ $0_str ],[ "(empty)" ])
    ])
    BC_MSG_RESULT([ $$0_str ])
    BS_UNSET([ $0_str ])
])


#   _VC_MSG_SUMMARY( LEVEL, HEADER )
#   ------------------------------------------------------------------------------------------------
#   Do not call it directly.
#   Internal guts of VC_MSG_SUMMARY. LEVEL is the message level, NOTICE or WARN.
#
BC_DEFUN([
    _VC_MSG_SUMMARY
],[
    VS_VAR_PUSHDEF([ log ],[ [_VC_]v4_trim([ $1 ])[_LOG] ])
    VS_VAR_IF_NOT_EMPTY([
        log
    ],[
        BS_ECHO([])
        BS_BOX( m4_normalize([ $2 ]) )
        BS_ECHO([])
        BS_ECHO([ "$log" ])
    ])
    VS_VAR_POPDEF([ log ])
])


#   VC_MSG_SUMMARY()
#   ------------------------------------------------------------------------------------------------
#   Prints all the notices (BC_MSG_NOTICE) and warnings (BC_MSG_WARN) collected so far.
#
BC_DEFUN([
    VC_MSG_SUMMARY
],[
    _VC_MSG_SUMMARY([ NOTICE ],[ Notices  ])
    _VC_MSG_SUMMARY([ WARN   ],[ Warnings ])
])


#   VC_CONFIG_AUX_DIR( DIR )
#   ------------------------------------------------------------------------------------------------
#   The same as AC_CONFIG_AUX_DIR, but stript leading and trailing whitespace from its argument,
#   and creates output variable auxdir.
#
BC_DEFUN([
    VC_CONFIG_AUX_DIR
],[
    BC_CONFIG_AUX_DIR([ $1 ])
    BC_SUBST([ auxdir ],[ $ac_aux_dir ])
])


#   BC_CONFIG_BUILDDIR()
#   ------------------------------------------------------------------------------------------------
#   Checks build dir. If build dir is the same as source dir, issues error message and stops
#   configure.
#
BC_DEFUN([
    VC_CONFIG_BUILDDIR
],[
    BS_VAR_SET([ BUILDDIR ],[ `pwd` ])
    VS_VAR_IF_EMPTY([
        BUILDDIR
    ],[
        VC_MSG_ERROR([ Can't find name of the currect directory ]) # '
    ])
    BS_VAR_SET([ SRCDIR ],[ `cd $srcdir && pwd` ])
    VS_VAR_IF_EMPTY([
        SRCDIR
    ],[
        VC_MSG_ERROR([ Can't find name of the source directory ]) # '
    ])
    BS_IF([
        test x"$BUILDDIR" = x"$SRCDIR"
    ],[
        VC_MSG_ERROR([
            In-source build is prohibited; create a build directory and run configure from within
            it, e. g.: mkdir _build && cd _build && ../configure
        ])
    ])
])


#   VC_META( META_FILE )
#   ------------------------------------------------------------------------------------------------
#   Parses META_FILE and extracts package name, version, release, copyright, license, home URL,
#   bug report URL, and email. It is expected RELEASE_FILE content looks like
#
#       Package         : Tapper
#       Version         : 0.2
#       Release         : 1
#       Copyright       : (C) 2007 John Doe
#       License         : GPLv3+
#       Home URL        : http://example.org/tapper
#       Bug report URL  : http://example.org/tapper/tracker
#       Email           : john.doe@example.org
#
#   Extracted parameters are saved in macros VC_PACKAGE_NAME, VC_PACKAGE_VERSION,
#   VC_PACKAGE_RELEASE, VC_PACKAGE_COPYRIGHT, VC_PACKAGE_LICENSE, VC_PACKAGE_URL,
#   VC_PACKAGE_BUGREPORT, VC_PACKAGE_EMAIL.
#
m4_define([VC_META],[
    m4_pushdef([content], m4_dquote(v4_include(v4_trim([ $1 ]))))
    dnl In the next line, \1 enclosed into 3 pair of brackets:
    dnl     1.  One pair is removed when collecting arguments of m4_bregexp;
    dnl     2.  One pair is removed when collecting arguments for m4_define;
    dnl.    3.  One pair remains to have package name quoted.
    m4_define([VC_PACKAGE_BUGREPORT],        m4_bregexp(content, [^ *Bug report URL *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_COPYRIGHT_YEAR],   m4_bregexp(content, [^ *Copyright year *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_COPYRIGHT_HOLDER], m4_bregexp(content, [^ *Copyright holder *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_DATE],             m4_bregexp(content, [^ *Date *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_EMAIL],            m4_bregexp(content, [^ *Email *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_ID],               m4_bregexp(content, [^ *Id *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_LICENSE],          m4_bregexp(content, [^ *License *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_LICENSE_URL],      m4_bregexp(content, [^ *License URL *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_NAME],             m4_bregexp(content, [^ *Package *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_RELEASE],          m4_bregexp(content, [^ *Release *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_SUMMARY],          m4_bregexp(content, [^ *Summary *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_URL],              m4_bregexp(content, [^ *Home URL *: *\(.*\)$], [[[\1]]]))
    m4_define([VC_PACKAGE_VERSION],          m4_bregexp(content, [^ *Version *: *\(.*\)$], [[[\1]]]))
    m4_ifblank(VC_PACKAGE_BUGREPORT,        [m4_fatal([$1: no bug report url found])])
    m4_ifblank(VC_PACKAGE_COPYRIGHT_YEAR,   [m4_fatal([$1: no copyright year found])])
    m4_ifblank(VC_PACKAGE_COPYRIGHT_HOLDER, [m4_fatal([$1: no copyright holder found])])
    m4_ifblank(VC_PACKAGE_DATE,             [m4_fatal([$1: no date found])])
    m4_ifblank(VC_PACKAGE_EMAIL,            [m4_fatal([$1: no email found])])
    m4_ifblank(VC_PACKAGE_ID,               [m4_fatal([$1: no id found])])
    m4_ifblank(VC_PACKAGE_LICENSE,          [m4_fatal([$1: no license found])])
    m4_ifblank(VC_PACKAGE_LICENSE_URL,      [m4_fatal([$1: no license url found])])
    m4_ifblank(VC_PACKAGE_NAME,             [m4_fatal([$1: no package name found])])
    m4_ifblank(VC_PACKAGE_RELEASE,          [m4_fatal([$1: no release found])])
    m4_ifblank(VC_PACKAGE_SUMMARY,          [m4_fatal([$1: no summary found])])
    m4_ifblank(VC_PACKAGE_URL,              [m4_fatal([$1: no home url found])])
    m4_ifblank(VC_PACKAGE_VERSION,          [m4_fatal([$1: no version found])])
    m4_cond(
        [m4_bregexp(VC_PACKAGE_NAME, [^[a-zA-Z0-9_ -]+$])],
        [-1],
        [m4_fatal([$1: invalid package name: ]VC_PACKAGE_NAME)])
    m4_cond(
        [m4_bregexp(VC_PACKAGE_ID, [^[a-zA-Z_-][a-zA-Z0-9_-]*\(\.[a-zA-Z_-][a-zA-Z0-9_-]*\)+$])],
        [-1],
        [m4_fatal([$1: invalid package id: ]VC_PACKAGE_ID)])
    m4_cond(
        [m4_bregexp(VC_PACKAGE_VERSION, [^[0-9]+\(\.[0-9]+\)*$])],
        [-1],
        [m4_fatal([$1: invalid version: ]VC_PACKAGE_VERSION))])
    m4_cond(
        [m4_bregexp(VC_PACKAGE_RELEASE, [^[0-9]+\(\.[0-9a-z]+\)*$])],
        [-1],
        [m4_fatal([$1: invalid release: ]VC_PACKAGE_RELEASE)])
    m4_cond(
        [v4_right(m4_expand([VC_PACKAGE_RELEASE]), [2])],
        [.t],
        [m4_define([VC_PACKAGE_TRIAL], [1])],
        [m4_define([VC_PACKAGE_TRIAL], [0])])
    m4_cond(
        [m4_bregexp(VC_PACKAGE_DATE, [^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]$])],
        [-1],
        [m4_fatal([$1: invalid release date: ]VC_PACKAGE_DATE)])
    m4_popdef([content])
    BC_PREREQ([ 2.62 ])
    BC_COPYRIGHT([ VC_PACKAGE_COPYRIGHT ])
])

#   AC_INIT(RELEASE_FILE)
#   ------------------------------------------------------------------------------------------------
#   AC_INIT can't be called from BC_INIT, because autoreconf wants to see AC_INIT macro directly in
#   configure.ac, otherwise it prints "not using Autoconf" and exits. So, I have to redefine
#   AC_INIT.
#
m4_define([VC_INIT], [
    # Define my extensions to Autoconf variables:
    BC_SUBST([ PACKAGE_COPYRIGHT_HOLDER ],[ 'VC_PACKAGE_COPYRIGHT_HOLDER'  ])
    BC_SUBST([ PACKAGE_COPYRIGHT_YEAR   ],[ 'VC_PACKAGE_COPYRIGHT_YEAR'    ])
    BC_SUBST([ PACKAGE_DATE             ],[ 'VC_PACKAGE_DATE'              ])
    BC_SUBST([ PACKAGE_EMAIL            ],[ 'VC_PACKAGE_EMAIL'             ])
    BC_SUBST([ PACKAGE_ID               ],[ 'VC_PACKAGE_ID'                ])
    BC_SUBST([ PACKAGE_LICENSE_URL      ],[ 'VC_PACKAGE_LICENSE_URL'       ])
    BC_SUBST([ PACKAGE_LICENSE          ],[ 'VC_PACKAGE_LICENSE'           ])
    BC_SUBST([ PACKAGE_RELEASE          ],[ 'VC_PACKAGE_RELEASE'           ])
    BC_SUBST([ PACKAGE_SUMMARY          ],[ 'VC_PACKAGE_SUMMARY'           ])
    BC_SUBST([ PACKAGE_TRIAL            ],[ 'VC_PACKAGE_TRIAL'             ])
    AH_TEMPLATE([PACKAGE_COPYRIGHT_HOLDER], [Copyright holder string])
    AH_TEMPLATE([PACKAGE_COPYRIGHT_YEAR],   [Copyright year string])
    AH_TEMPLATE([PACKAGE_DATE],             [Release date])
    AH_TEMPLATE([PACKAGE_ID],               [Application Id])
    AH_TEMPLATE([PACKAGE_LICENSE],          [Package license])
    AH_TEMPLATE([PACKAGE_LICENSE_URL],      [Package license URL])
    AH_TEMPLATE([PACKAGE_RELEASE],          [Package release])
    AH_TEMPLATE([PACKAGE_SUMMARY],          [Package summary])
    AH_TEMPLATE([PACKAGE_TRIAL],            [Is package trial?])
    AC_DEFINE([PACKAGE_COPYRIGHT_YEAR],   ["VC_PACKAGE_COPYRIGHT_YEAR"])
    AC_DEFINE([PACKAGE_COPYRIGHT_HOLDER], ["VC_PACKAGE_COPYRIGHT_HOLDER"])
    AC_DEFINE([PACKAGE_DATE],             ["VC_PACKAGE_DATE"])
    AC_DEFINE([PACKAGE_ID],               ["VC_PACKAGE_ID"])
    AC_DEFINE([PACKAGE_LICENSE_URL],      ["VC_PACKAGE_LICENSE_URL"])
    AC_DEFINE([PACKAGE_LICENSE],          ["VC_PACKAGE_LICENSE"])
    AC_DEFINE([PACKAGE_RELEASE],          ["VC_PACKAGE_RELEASE"])
    AC_DEFINE([PACKAGE_SUMMARY],          ["VC_PACKAGE_SUMMARY"])
    AC_DEFINE([PACKAGE_TRIAL],            [VC_PACKAGE_TRIAL])
    #~ AC_DEFINE([PACKAGE_EMAIL], ["VC_PACKAGE_EMAIL"])
])


#   BC_CHECK_RPM_DIST()
#   ------------------------------------------------------------------------------------------------
#   Find out value of rpmbuild's dist macro. Sets shell and output variable RPM_DIST.
#
BC_DEFUN([
    VC_CHECK_RPM_DIST
],[
    # TODO: Cache it.
    BC_MSG_CHECKING([ for rpmbuild dist ])
    BS_VAR_SET([ RPM_DIST ],[ `$RPMBUILD --eval %{?dist:%{dist}}` ])
    VC_MSG_RESULT([ $RPM_DIST ])
    BC_SUBST([ RPM_DIST ])

    BC_MSG_CHECKING([ for rpmbuild disttag ])
    BS_VAR_SET([ RPM_DISTTAG ],[ `$RPMBUILD --eval %{?disttag:-%{disttag}}` ])
    VC_MSG_RESULT([ $RPM_DISTTAG ])
    BC_SUBST([ RPM_DISTTAG ])
])


#   BC_CHECK_RPM_ARCH()
#   ------------------------------------------------------------------------------------------------
#   Find out value of rpmbuild's _arch macro. Sets shell and output variable RPM_ARCH.
#
BC_DEFUN([
    VC_CHECK_RPM_ARCH
],[
    # TODO: Cache it.
    BC_MSG_CHECKING([ for rpmbuild arch ])
    BS_VAR_SET([ RPM_ARCH ],[ `$RPMBUILD --eval %{?_arch:%{_arch}}` ])
    VC_MSG_RESULT([ $RPM_ARCH ])
    BC_SUBST([ RPM_ARCH ])
])


#   VC_CHECK_PROG( VAR, HELP, PROGRAMS, COMMANDS_IF_FOUND, COMMANDS_IF_NOT_FOUND )
#   ------------------------------------------------------------------------------------------------
#   AC_PATH_PROG and AC_PATH_PROGS (to my taste) has few disadvantages:
#
#       1.  If VAR is already set to an absolute path, AC_PATH_PROG prints "Checking for PROGRAM...
#           VAR" but does *nothing*. It is ok if VAR was set by AC_PATH_PROG called before, but it
#           is not ok if the variable is set by user in the command line (e. g. VAR=/bin/prog
#           ./configure), because check completes successfuly, even if program does not exist.
#
#       2.  AC_PATH_PROGS allows author to specify program with arguments, e. g.:
#
#               AC_PATH_PROG([VAR],["prog1 -q" "prog2 -v"])
#
#           AC_PATH_PROGS returns absolute path of the first found program, but it silently strips
#           down all the specified options.
#
#       3.  If user specify desired program *with arguments* in the environment variable (e. g.
#           VAR="/bin/prog -q" ./configure), both AC_PATH_PROG and AC_PATH_PROGS silently strips
#           down all the arguments.
#
#   VC_CHECK_PROG fixes these disadvantages. Oh! It also documents VAR by calling BC_ARG_VAR.
#
#   Examples:
#
#       Check for mandatory program:
#           VC_CHECK_PROG([ PERL ],[ Perl interpreter ],[ perl ],[],[
#               AC_MSG_ERROR([ perl not found ])
#           ])
#       Note AC_MSG_ERROR — it stops configure, so missed perl will be fatal.
#
#       Check for optional program:
#           VC_CHECK_PROG([ POD2HTML ],[ pod2html program ],[ pod2html ],[],[
#               VC_MSG_WARNING([ pod2html not found ])
#           ])
#       Note VC_MSG_WARNING — it does not stop configure, so missed pod2html will not be fatal.
#
#   TODO: Default value for $2?
#
BC_DEFUN([
    VC_CHECK_PROG
],[
    VS_VAR_PUSHDEF(
        [ $0_variable  ],[ v4_trim([ $1 ])               ],
        [ $0_cache     ],[ [vc_cv_path_]$0_variable      ],
        [ $0_shadow    ],[ [$0_]$0_variable[__shadow]    ],
        [ $0_command   ],[ [$0_]$0_variable[__command]   ],
        [ $0_program   ],[ [$0_]$0_variable[__program]   ],
        [ $0_arguments ],[ [$0_]$0_variable[__arguments] ]
    )
    BC_ARG_VAR( $0_variable ,[ $2 ])
    VS_VAR_IF_NOT_SET([
        $0_cache
    ],[
        VS_VAR_IF_SET([
            $0_variable
        ],[
            # Show environment variable and its value:
            BC_MSG_CHECKING([ for \$$0_variable ])
            VC_MSG_RESULT([ $$0_variable ])
            # Save variable value and clear original variable,
            # otherwise BC_PATH_PROG will do nothing:
            BS_VAR_COPY([ $0_shadow ],[ $0_variable ])
            BS_UNSET([ $0_variable ])
        ],[
        ])
        for $0_command in "$$0_shadow" $3
        do
            VS_VAR_IF_NOT_EMPTY([
                $0_command
            ],[
                # We have a command, find out program (the first word)
                # and arguments (the rest).
                set dummy $$0_command
                shift
                BS_VAR_COPY([ $0_program ],[ 1 ])
                shift
                BS_VAR_COPY([ $0_arguments ],[ @ ])
                VS_IF_ABS_PATH([
                    $$0_program
                ],[
                    # Program is set as absolute path. AS_PATH_PROG does nothing in such a case,
                    # so we have to perform the check manually.
                    BC_MSG_CHECKING([ $$0_program ])
                    BS_IF([
                        BS_EXECUTABLE_P([ $$0_program ])
                    ],[
                        VC_MSG_RESULT([ yes ])
                        BS_VAR_SET([ $0_variable ],[ $0_program ])
                    ],[
                        VC_MSG_RESULT([ no ])
                    ])
                ],[
                    # Let BC_PATH_PROG do the job:
                    BC_PATH_PROG( $0_variable ,[ $$0_program ])
                ])
                VS_VAR_IF_NOT_EMPTY([
                    $0_variable
                ],[
                    # Restore arguments:
                    VS_VAR_IF_NOT_EMPTY([
                        $0_arguments
                    ],[
                        BS_VAR_APPEND([ $0_variable ],[ " $$0_arguments" ])
                    ])
                    break
                ])
                VS_VAR_IF_SET([
                    $0_shadow
                ],[
                    break
                ])
            ])
        done
        BS_VAR_COPY([ $0_cache ],[ $0_variable ])
    ])
    VS_VAR_UNSET([
        $0_shadow
        $0_command
        $0_program
        $0_arguments
    ])
    VS_VAR_IF_NOT_EMPTY([
        $0_cache
    ],[
        $4
    ],[
        $5
    ])
    VM_CONDITIONAL( $0_variable )
    VS_VAR_POPDEF([
        $0_variable
        $0_cache
        $0_shadow
        $0_command
        $0_program
        $0_arguments
    ])
])

#   VC_CHECK_PROG_AUTO( FEATURE, VAR, HELP, PROGRAMS )
#   ------------------------------------------------------------------------------------------------
#   Conditional check for a program. FEATURE is a feature name declared by VC_ARG_ENABLE. If
#   feature is disabled (e. g. feature variable is "no"), check is completely skipped, otherwise
#   check is performed. If program is found, feature variable is updated to "yes".
#
#   Example:
#       VC_ARG_ENABLE([ rpm ],[ auto yes no ],[
#           Enable/disable building and testing rpm packages.
#           auto == enable rpm if rpmbuild program found.
#       ])
#       VC_CHECK_PROG_AUTO([ rpm ],[ RPMBUILD ],[ rpmbuild program ],[ rpmbuild ])
#
BC_DEFUN([
    VC_CHECK_PROG_AUTO
],[
    VS_VAR_PUSHDEF(
        [ $0_feature_name  ],[ v4_trim([ $1 ])          ],
        [ $0_feature_var   ],[ [enable_]v4_trim([ $1 ]) ]
    )
    BS_CASE([
        "$$0_feature_var"
    ],[
        yes|auto
    ],[
        VC_CHECK_PROG([ $2 ],[ $3 ],[ $4 ],[
            BS_VAR_IF([
                $0_feature_var
            ],[
                "auto"
            ],[
                # Program found, set feature variable to "yes".
                VC_MSG_RESULT([
                    --enable-$0_feature_name=auto, $3 found => $0_feature_name enabled
                ])
                BS_VAR_SET([ $0_feature_var ],[ "yes" ])
            ])
        ],[
            # Program not found...
            BS_VAR_IF([
                $0_feature_var
            ],[
                "yes"
            ],[
                # Feature is enabled => issue error.
                VC_MSG_ERROR([ --enable-$0_feature_name=yes => $3 is a must ])
            ],[
                # Feature is auto => issue warning and disable feature.
                VC_MSG_WARN([ $3 not found => $0_feature_name [disabled] ])
                BS_VAR_SET([ $0_feature_var ],[ "no" ])
            ])
        ])
    ])
    VM_CONDITIONAL([ enable_]$0_feature_name[ ])
    VS_VAR_POPDEF([
        $0_feature_name
        $0_feature_var
    ])
])


# end of file #
