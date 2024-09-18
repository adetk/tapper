#!/bin/bash

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/install-exec-hook.sh
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

eval "$PROLOGUE"

[[ $# -ge 1 ]] || die "No argument"
[[ $# -le 1 ]] || die "Too many arguments"

if [[ -n $WITH_LIBCAP ]]; then
    [[ -n "$SETCAP" ]] || skip "SETCAP is not set"
else
    [[ -n "$CHGRP" ]] || skip "CHGRP is not set"
    [[ -n "$CHMOD" ]] || skip "CHMOD is not set"
fi

if [[ -n $ENABLE_EMITTERS && -n $WITH_LIBEVDEV ]]; then
    if [[ -n $WITH_LIBCAP ]]; then
        $SETCAP 'cap_setuid,cap_setgid=p' $1
    else
        $CHOWN root $1
        $CHMOD u+s $1
    fi
elif [[ -n $WITH_LIBINPUT ]]; then
    if [[ -n $WITH_LIBCAP ]]; then
        $SETCAP 'cap_setgid=p' $1
    else
        $CHGRP input $1
        $CHMOD g+s $1
    fi
fi

exit 0

# end of file #
