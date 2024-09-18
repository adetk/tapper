#!/bin/bash

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/check-rpm-rpmlint.sh
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
log0="$tmpdir/$( basename "$1" ).log"
log1="$tmpdir/$( basename "$1" ).filtered.log"

#   rpmlint exit statuses:
#        0  No errors.
#        1  Unspecified error.
#        2  Interrupted.
#       64  One or more error message printed.
#       66  Badness threshold exceeded.

#   grep returns status 0 if there are some output, 1 if output is empty and > 1 in case of errors.
#   In our case empty output means everything is ok (no warning no error messages), so we need to
#   invert grep status, but not ignore errors.

check=0
call RPMLINT --strict --rpmlintrc="$SRCDIR/etc/rpmlint.conf" "$@" > "$log0" 2>&1 || check=$?
#   rpmlint 2.2.0 has a number of problems:
#       1.  It is too talkative. It prints version, configuration, etc, so its output is never
#           empty.
#       2.  It is very hard to get rid of `unused-rpmlintrc-filter` error. The same rpmlintrc file
#           could be ok for source rpm but cause `unused-rpmlintrc-filter` error for binary rpm.

if [[ $check -ne 0 ]]; then
    # Something happened.
    if [[ $check -eq 64 || $check -eq 66 ]]; then
        # rpmlint found some errors, but let's filter them first:
        egrep -v -e ": E: unused-rpmlintrc-filter " "$log0" > "$log1" || :
        grep=0
        egrep -e ": (E|W): " "$log1" || grep=$?
        if [[ $grep -eq 0 ]]; then
            fail # grep found some errors.
        else
            pass # grep found no errors.
        fi
    else
        cat "$log0"
        die "rpmlint exited with status $rpmlint"
    fi
fi

pass

# end of file #
