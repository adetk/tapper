#!/bin/sh

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/check-srpm-rpmbuild.sh
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

mktmpdir
topdir=$( realpath "$tmpdir" )      # rpmbuild does not like relative pathnames.

# Note: This script is called from "outer" make, and rpmbuild invokes "inner" make.
# It causes problem, because outer make saves some flags (including a list of tests to run) in
# environment variable MAKEFLAGS. Thus, inner make tries to execute the same tests as outer one,
# but it should not, because in spec file I explicitly disabled html and rpm. Clearing MAKEFLAGS
# variable avoids the problem.
MAKEFLAGS= \
call RPMBUILD \
    --noclean \
    -D "_topdir $topdir" \
    -rb "$1"

# end of file #
