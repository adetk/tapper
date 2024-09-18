#!/bin/bash

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/clean.sh
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

set -eo pipefail
. $( dirname "$0" )/prologue.sh

function help() {
    #     12345678901234567890123456789012345678901234567890123456789012345678901234567890
    echo "Usage:"
    echo "    $script [OPTION]... [DIR|FILE]..."
    echo "Options:"
    echo "    -n, --dry-run"
    echo "        Show files/directories to be removed but do not remove them actually."
    echo "    -r, --remove"
    echo "        Remove specified files/directories instead of cleaning them."
    echo "    -v, --verbose"
    echo "        Be more verbose."
    echo "    -x, --exclude=PATTERN"
    echo "        Do not remove files/directories that match given pattern (in clean mode"
    echo "        only)."
    echo "    -h, --help"
    echo "        Print this help message and exit."
    echo "Description:"
    echo "    The script works in two modes: 'clean' (default) and 'remove'. In clean mode"
    echo "    the script removes the content of the given directories. All the given"
    echo "    directories must exist. Some files/subdirs may be protected from removal by"
    echo "    using -x option. In remove mode the script simply removes the given files"
    echo "    and directories. Non-existent files or directories do not cause errors. In"
    echo "    both modes the script makes the given files and directories writable for the"
    echo "    user in order to remove read-only content."
    echo "Example:"
    echo "    Clean _build/ directory, keep any config.* files if any:"
    echo "        $script -x 'config.*' _build"
    echo "    Remove _build/ directory:"
    echo "        $script -r _build"
}

function match() {
    local item pattern
    item="$1"
    shift 1
    for pattern in "$@"; do
        case "$item" in
            ( $pattern ) return 0;;
            ( *        ) :;;
        esac
    done
    return 1
}

#
#   Parse command line.
#

remove=
exclude=()
run=""
verbose=""

eval set -- \
    $(                                                            \
        /bin/getopt -n "$script" --options "rnx:v"                \
            --long "help,remove,dry-run,exclude:,verbose"         \
            -- "$@"                                               \
    )
while true; do
    case "$1" in
        ( -h | --help )
            help
            exit 0
            ;;
        ( -n | --dry-run )
            run="echo"
            shift 1
            ;;
        ( -r | --remove )
            remove=1
            shift 1
            ;;
        ( -x | --exclude )
            exclude+=( "$2" )
            shift 2
            ;;
        ( -v | --verbose )
            verbose=v
            shift 1
            ;;
        ( -- )
            shift 1
            break
            ;;
        ( * )
            die "oops"
            ;;
    esac
done
if [[ -n "$remove" && ${#exclude[@]} -gt 0 ]]; then
    die "Options --remove end --exclude are mutually exclusive."
fi

#
#   Do the work.
#

for arg in "$@"; do
    if [[ -n "$remove" ]]; then
        if [[ -e "$arg" ]]; then
            $run /bin/chmod -R u+w "$arg"
            $run /bin/rm -rf$verbose -- "$arg"
        else
            : # If a directory to remove does not exist, it is not an error.
        fi
    else
        dir=$arg
        [[ -e "$dir" ]] || die "$dir: does not exist"
        [[ -d "$dir" ]] || die "$dir: not a directory"
        $run /bin/chmod -R u+w "$dir"
        /bin/ls -a -1 "$dir" | \
        while read file; do
            if match "$file" "." ".." "${exclude[@]}"; then
                if [[ -n "$verbose" ]]; then
                    echo "skipped '$dir/$file'"
                fi
            else
                $run /bin/rm -rf$verbose -- "$dir/$file"
            fi
        done
    fi
done

exit 0

# end of file #
