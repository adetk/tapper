#!/bin/bash

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/check-metainfo-appstream.sh
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

[[ $# -gt 0 ]] || die "No argument"
[[ $# -le 1 ]] || die "Too many arguments"

# When building RPM with `mock`, network can be enabled or disabled.
# How to detect it?
if [[ -n $AUTHOR_TESTING ]]; then
    opts=()
else
    opts=( --no-net )
fi

version=$( call APPSTREAM --version )
[[ $version =~ ^AppStream\ version:\ (.*)$ ]] || die "Can't parse appstream version: $version"
version=${BASH_REMATCH[1]}

status=0
run call APPSTREAM validate "${opts[@]}" "$1" || status=$?
# `appstream` exits with status 3 in case of warnings.
[[ $status -eq 0 || $status -eq 3 ]] || fail

#   Messages look like:
#       I: io.sourceforge.kbd-tapper:20: cid-contains-hyphen io.sourceforge.kbd-tapper
#       W: io.sourceforge.kbd-tapper:61: tag-empty description
#       E: io.sourceforge.kbd-tapper:61: metainfo-localized-description-tag description

# I know about hyphen in id, let me ignore this message for now:
egrep -v -e '^I: .*: cid-contains-hyphen .*$' "$tmpfile.out" > "$tmpfile.1.tmp" || :

# Old appstreamcli 0.12.9 in centos-stream+epel-next-8 complains on <control> tag:
if vle "$version" "0.12.9"; then
    egrep -v -e '^W: .*: relation-invalid-tag control' "$tmpfile.1.tmp" > "$tmpfile.2.tmp" || :
else
    cp "$tmpfile.1.tmp" "$tmpfile.2.tmp"
fi

# But if there are any more messages, let the test fail:
! egrep -e '^[IWE]: ' "$tmpfile.2.tmp"

# end of file #
