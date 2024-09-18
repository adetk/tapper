#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: m4/vm.m4
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
#   Some Automake extensions.
#

#   VM_CONDITIONAL( VAR_1, VAR_2, ... )
#   ------------------------------------------------------------------------------------------------
#   Like AM_CONDITINAL, but does not require condition. Make conditional variable defined if shell
#   variable with the same name is not empty and is not "no".
#
m4_define(
    [_vm_conditional],
    [BC_CONFIG_COMMANDS_PRE([
        if test -z "${$1_TRUE}" && test -z "${$1_FALSE}"; then
            $1_TRUE='#'
            $1_FALSE=
        fi
    ])BM_CONDITIONAL([
        $1
    ],[
        VS_VAR_IF_NOT_EMPTY([
            $1
        ],[
            BS_VAR_IF([
                $1
            ],[
                "no"
            ],[
                false
            ],[
                :
            ])
        ],[
            false
        ])
    ])])
m4_define(
    [VM_CONDITIONAL],
    [m4_map_args_w([$1], [_vm_conditional(], [)])])


# end of file #
