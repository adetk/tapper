#!/bin/sh

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/check-cpp-cppcheck.sh
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

test $# -gt 0 || die "No argument"
test $# -le 1 || die "Too many arguments"

test -n "$AUTHOR_TESTING" || skip "AUTHOR_TESTING is not set"

call CPPCHECK \
    -v \
    --error-exitcode=1 \
    --template=gcc \
    --inline-suppr \
    --enable=warning,style,performance,portability \
    -I . \
    "$1"

pass

# end of file #
