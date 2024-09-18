#!/bin/bash

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/check-html-tidy.sh
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

mktmpdir
log="$tmpdir/$( basename "$1" ).log"

# tidy returns status 1 if there are any warnings or 2 if there are any errors (which may be
# either errors in html code or command line or runtime errors).
# However, if tidy print some accessibility warnings, it returns status 0.

tidy=0
call TIDY -q -e -utf8 "$@" > "$log" 2>&1 || tidy=$?
if [[ $tidy == 0 ]]; then
    if [[ -s $log ]]; then
        # tidy returned status 0 but log file is not empty.
        # Let's think it printed some accessibility warnings.
        tidy=1
    fi
fi

# I want to filter out following warnings:
filter=(
    '<a> escaping malformed URI reference'
        # `tidy` does not like Russian letters in anchor names.
    '<ol> attribute "type" not allowed for HTML5'
        # `pandoc` generates such HTML code, it's out of my control.
)
grep=0
if [[ ${#filter[*]} -gt 0 ]]; then
    IFS='|' filter="(${filter[*]})"
    grep -E -v -e "$filter" "$log" || grep=$?
else
    cat "$log"
fi

if [[ $tidy -eq 1 && $grep -eq 1 ]]; then
    # Tidy reported some warnings but we filtered out all the warnings.
    pass
fi
exit $tidy

# end of file #
