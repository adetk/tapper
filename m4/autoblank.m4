#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: m4/autoblank.m4
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
#   Autoconf, M4sh, Automake macro wrappers.
#
#   B?_* macros are the same as their A?_* counterparts. The only difference is that B?_* macros
#   trim leading and trailing whitespaces (including quoted) from all the arguments, allowing a bit
#   more free formatting:
#
#       AX_MACRO(                       BX_MACRO([
#           [arg1],                         arg1
#           [arg2])                     ],[
#                                           arg2
#                                       ])
#
#   This does not look like a big advantage in trivial cases. However, in case of nested macro
#   calls trail of closing parenthesis and brackets grows rapidly.
#


m4_pushdef([define_macro],
    [m4_define(m4_bpatsubst([[$1]], [^\(.\)A], [\1B]), [$1(v4_atrim($][@))])])
m4_map_args_w(
    dnl # Autoconf macros:
    [
        AC_ARG_ENABLE
        AC_ARG_VAR
        AC_ARG_WITH
        AC_CACHE_VAL
        AC_CHECK_HEADER
        AC_CONFIG_AUX_DIR
        AC_CONFIG_COMMANDS_PRE
        AC_CONFIG_FILES
        AC_CONFIG_HEADERS
        AC_CONFIG_MACRO_DIR
        AC_CONFIG_SRCDIR
        AC_COPYRIGHT
        AC_DEFINE
        AC_DEFUN
        AC_DEFUN_ONCE
        AC_MSG_CHECKING
        AC_MSG_ERROR
        AC_MSG_NOTICE
        AC_MSG_RESULT
        AC_MSG_WARN
        AC_OUTPUT
        AC_PATH_PROG
        AC_PATH_PROGS
        AC_PATH_PROGS_FEATURE_CHECK
        AC_PREREQ
        AC_SUBST
    ]
    dnl # M4sh macros:
    [
        AS_BOX
        AS_IF
        AS_CASE
        AS_DIRNAME
        AS_ECHO
        AS_ECHO_N
        AS_ESCAPE
        AS_EXECUTABLE_P
        AS_EXIT
        AS_LITERAL_IF
        AS_LITERAL_WORD_IF
        AS_MKDIR_P
        AS_SET_CATFILE
        AS_SET_STATUS
        AS_TR_CPP
        AS_TR_SH
        AS_UNSET
        AS_VAR_APPEND
        AS_VAR_ARITH
        AS_VAR_COPY
        AS_VAR_IF
        AS_VAR_POPDEF
        AS_VAR_PUSHDEF
        AS_VAR_SET
        AS_VAR_SET_IF
        AS_VAR_TEST_SET
        AS_VERSION_COMPARE
        AS_HELP_STRING
    ]
    dnl # Automake macros:
    [
        AM_CONDITIONAL
        AM_INIT_AUTOMAKE
        AM_MAINTAINER_MODE
    ]
    ,
    [define_macro(],
    [)]
)
m4_popdef([define_macro])

#
#   Some special cases to fix Autoconf irregularities.
#

m4_pushdef([define_macro],
    [_define_macro(v4_atrim($@))])
m4_pushdef([_define_macro],
    [m4_define([_$1], m4_defn([$1]))m4_ignore(
    )m4_define([$1], [_$1($2)])])


#   BS_IF( CONDITION_1, THEN_1, CONDITION_2, THEN_2, ..., ELSE )
#   ------------------------------------------------------------------------------------------------
#   AS_IF generates : statement in the beginning of every THEN_*, but not in ELSE. Thus, if ELSE
#   consists of comments with no executable statements, e. g.:
#
#       BS_IF([
#           true
#       ],[
#           ...
#       ],[
#           # Do noting
#       ])
#
#   AS_IF generates invalid code:
#
#       if true; then :
#           ...
#       else
#           # Do nothing
#       fi
#
#   Shell requires at least one command in else part, so generated code above will not work. Let us
#   add : statement to the beginning of ELSE, it will fix the problem.
#
define_macro([
    BS_IF
],[
    [ $1 ],
    [ $2 ]m4_ignore(
    )m4_map_args_pair(
        [, m4_echo],
        [, [:]m4_newline([  ])v4_trim],
        m4_shift2($@))
])


#
#   BS_CASE( EXPRESSION, PATTERN_1, COMMANDS_1, PATTERN_2, COMMANDS_2, ..., COMMANDS_DEFAULT )
#   ------------------------------------------------------------------------------------------------
#   BS_CASE trims trailing newlines from all the arguments. If any COMMAND_* parameter ends with
#   a comment, it has disaster effect: AS_CASE simply appends ;; (and some more code) to the end of
#   COMMANDS_*... which appears inside the comment. To avoid this unwanted behavior, let us append
#   newline and : statement to the end of each COMMAND_* — those newlines are not a traling ones
#   and so will not be trimmed by BS_CASE.
#
define_macro([
    BS_CASE
],[
    [ $1 ]m4_ignore(
    )m4_map_args_pair(
        [, _bs_case_args],
        [],
        m4_shift($@))
])
m4_define([_bs_case_args], [v4_atrim($@)m4_newline([    :])])


#   BC_ARG_ENABLE( FEATURE, HELP, COMMANDS_IF_GIVEN, COMMANDS_IF_NOT )
#   ------------------------------------------------------------------------------------------------
#   AC_ARG_ENABLE uses AS_IF to generate conditional statement and suffers from problem of AS_IF:
#   if COMMANDS_IF_NOT consists of comments with no executbale code, it causes incorrect shell
#   code.
#
define_macro([
    BC_ARG_ENABLE
],[
    [ $1 ],
    [ $2 ],
    [ $3 ],
    [:]m4_newline([  ])v4_trim([ $4 ])
])


m4_popdef([define_macro])
m4_popdef([_define_macro])

# end of file #
