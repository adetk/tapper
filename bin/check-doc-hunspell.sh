#!/bin/sh

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/check-doc-hunspell.sh
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
test -n "$HUNSPELL"       || skip "HUNSPELL is not set"
test -n "$PANDOC"         || skip "PANDOC is not set"
test -n "$PERL"           || skip "PERL is not set"

path="$1"
base=$( basename "$path" )

bad_words="$base.hunspell.tmp"
trap 'rm -f "$bad_words"' EXIT

cat_man() {
    # man argument must contain at least one slash to be interpreted as file path.
    # Looks like code samples starts with 14 spaces, filter such lines out.
    man $( realpath "$1" ) | grep -E -v "^              "
};

cat_md() {
    # Let's drop pieces of code: no need to check spelling of the code.
    cat "$1" \
    | \
    $PERL -p \
        -e 'BEGIN { $/ = undef; }' \
        -e 's{`[^`]*`}{ CODE }gms;'  \
        -e 's{^    \$(.*?\\\n)*.*?\n}{    \$ CODE\n}gm;'  \
    | \
    $PANDOC -f markdown-smart -t plain
};

cat_txt() {
    cat "$1"
};

spell_check() {
    $HUNSPELL -d en_US -p "$SRCDIR/etc/en_US.dic" -l \
    | \
    sort \
    | \
    uniq \
    | \
    tee "$bad_words"
};


case "$base" in
    ( GPL-3.0.md )
        skip "This is not my file";;
    ( *.man )
        cat_man "$path" | spell_check;;
    ( *.md )
        cat_md  "$path" | spell_check;;
    ( *.txt )
        cat_txt "$path" | spell_check;;
    ( * )
        die "Do not know how to check '$path'";;
esac
[[ ! -s $bad_words ]] || fail

# end of file #
