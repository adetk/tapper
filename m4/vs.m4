#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: m4/vs.m4
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
#   Some M4sh extensions.
#

#   VS_VAR_PUSHDEF( VAR_1, NAME_1, VAR_2, NAME_2, ... )
#   ------------------------------------------------------------------------------------------------
#   Like BS_VAR_PUSHDEF, but can handle multiple variable definitions.
#
m4_define(
    [VS_VAR_PUSHDEF],
    [m4_map_args_pair([BS_VAR_PUSHDEF], [m4_fatal([$0: odd number of arguments])], $@)])


#   VS_VAR_POPDEF( VAR_1, VAR_2, ... )
#   ------------------------------------------------------------------------------------------------
#   Like BS_VAR_POPDEF, but can handle multiple variables. Variables can be passed as separate
#   arguemnts or as whitespace-separated list, or mix of it:
#
#       VS_VAR_POPDEF([ command program arguments ],[ variable ])
#
m4_define(
    [VS_VAR_POPDEF],
    [m4_map_args([_vs_var_popdef], $@)])
m4_define(
    [_vs_var_popdef],
    [m4_map_args_w([$1], [BS_VAR_POPDEF(], [)])])


#   VS_VAR_UNSET( VAR_1, VAR_2, ... )
#   ------------------------------------------------------------------------------------------------
#   Like BS_UNSET, but can handle multiple variables. Variables can be passed as separate arguemnts
#   or as whitespace-separated list, or mix of it:
#
#       VS_VAR_UNSET([ command program arguments ],[ variable ])
#
m4_define(
    [VS_VAR_UNSET],
    [m4_map_args([_vs_var_unset], $@)])
m4_define(
    [_vs_var_unset],
    [m4_map_args_w([$1], [BS_UNSET(], [)v4_nl])])


#   VS_VAR_IF_SET( VAR, COMMANDS_THEN, COMMANDS_ELSE )
#   ------------------------------------------------------------------------------------------------
#   The same as BS_VAR_SET_IF, which name always confuses me.
#
m4_define(
    [VS_VAR_IF_SET],
    [BS_VAR_SET_IF([ $1 ],[ $2 ],[ $3 ])])


#   VS_VAR_IF_NOT_SET( VAR, COMMANDS_THEN, COMMANDS_ELSE )
#   ------------------------------------------------------------------------------------------------
#   The same as VS_VAR_IF_SET, but with negated condition.
#
m4_define(
    [VS_VAR_IF_NOT_SET],
    [BS_VAR_SET_IF([ $1 ],[ $3 ],[ $2 ])])


#   VS_VAR_IF_EMPTY( VAR, COMMANDS_THEN, COMMANDS_ELSE )
#   ------------------------------------------------------------------------------------------------
#
m4_define(
    [VS_VAR_IF_EMPTY],
    [BS_VAR_IF([ $1 ],[ "" ],[ $2 ],[ $3 ])])

#   VS_VAR_IF_NOT_EMPTY( VAR, COMMANDS_THEN, COMMANDS_ELSE )
#   ------------------------------------------------------------------------------------------------
#
m4_define(
    [VS_VAR_IF_NOT_EMPTY],
    [BS_VAR_IF([ $1 ],[ "" ],[ $3 ],[ $2 ])])

#   VS_VAR_IF_ENABLED( VAR, COMMANDS_THEN, COMMANDS_ELSE )
#   ------------------------------------------------------------------------------------------------
#
m4_define(
    [VS_ARG_IF_ENABLED],
    [BS_VAR_IF( [enable_]v4_trim([ $1 ]),[ "yes" ],[ $2 ],[ $3 ])])

#   VS_VAR_IF_WITH( VAR, COMMANDS_THEN, COMMANDS_ELSE )
#   ------------------------------------------------------------------------------------------------
#
m4_define(
    [VS_ARG_IF_WITH],
    [BS_VAR_IF( [with_]v4_trim([ $1 ]),[ "yes" ],[ $2 ],[ $3 ])])





#   DS_IF_ENABLED( FEATURE, THEN, ELSE )
#   ------------------------------------------------------------------------------------------------
#   Similar to BS_IF, checks if FEATURE is enabled. Example:
#
#       BC_ARG_ENABLE([ rpm ], ...)
#       ...
#       DS_IF_ENABLED([
#           rpm
#       ],[
#           THEN
#       ],[
#           ELSE
#       ])
#
BC_DEFUN([
    DS_IF_ENABLED
],[
    BS_IF([
        test x"$enable_]v4_trim([ $1 ])[" == x"yes"
    ],[
        $2
    ],[
        $3
    ])
])


#   DS_IF_ABS_PATH( FILENAME, THEN, ELSE )
#   ------------------------------------------------------------------------------------------------
#
#       DS_IF_ABS_PATH([
#           FILENAME
#       ],[
#           THEN
#       ],[
#           ELSE
#       ])
#
BC_DEFUN([
    VS_IF_ABS_PATH
],[
    BS_CASE([
        $1
    ],[
        /* | \\* | ?:/* | ?:\\*
    ],[
        $2
    ],[
        $3
    ])
])


# end of file #
