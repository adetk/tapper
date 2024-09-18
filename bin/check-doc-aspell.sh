#!/bin/sh

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/check-doc-aspell.sh
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

path="$1"
base=$( basename "$path" )

bad_words="$base.hunspell.tmp"
trap 'rm -f "$bad_words"' EXIT

mktmpdir

cat_man() {
    local path base
    path="$1"
    base=$( basename "$path" )
    # man argument must contain at least one slash to be interpreted as file path.
    # Looks like code samples starts with 14 spaces, filter such lines out.
    call MAN $( realpath "$1" ) | grep -E -v "^              " > "$tmpdir/$base"
    cat "$tmpdir/$base"
};

cat_md() {
    local path base
    path="$1"
    base=$( basename "$path" )
    # Let's drop comments and pieces of code: no need to check spelling of the code.
    call PERL -p \
        -e 'BEGIN { $/ = undef; }' \
        -e 's{(`[^`]+`)}{<!-- CODE -->}gms;'  \
        -e 's{^ {4,}\$.*\n( {4,}.*\n)*}{<!-- CODE -->\n}gm;'  \
        "$path" \
    > "$tmpdir/$base"
    call PANDOC -f markdown-smart -t plain "$tmpdir/$base"
};

cat_txt() {
    cat "$1"
};

spell_check() {
    call ASPELL --encoding=UTF-8 -d en_US -l en -p "./en.pws" list \
    | \
    sort \
    | \
    uniq \
    | \
    tee "$bad_words"
};


case "$base" in
    GPL-3.0.md )
        skip "This is not my file";;
    *.man )
        cat_man "$path" | spell_check;;
    *.md )
        cat_md  "$path" | spell_check;;
    *.txt )
        cat_txt "$path" | spell_check;;
    * )
        die "Do not know how to check '$path'";;
esac
test ! -s "$bad_words" || fail

# end of file #
