#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/prologue.sh
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

script=$( basename "$0" )
tmpdir="$script.tmp"
tmpfile="$script.$$"

cleanup=()
cleanup() {
    rm -rf "$tmpdir" "$tmpfile".* "${cleanup[@]}"
}

trap cleanup EXIT

say() {
    local line
    for line in "$@"; do
        if [[ -n "$line" ]]; then
            echo "$script: $line" >&2
        else
            echo "" >&2
        fi
    done
}

pass() {
    say "$@"
    exit 0
}

fail() {
    say "$@"
    exit 1
}

die() {
    say "$@"
    exit 99
}

skip() {
    say "$@"
    exit 77
}

mktmpdir() {
    [[ -d "$tmpdir" ]] || mkdir "$tmpdir"
}

#   ------------------------------------------------------------------------------------------------
#   Name:
#       call — run a command indirectly, through an environment varible
#   Usage:
#       call VARIABLE options… arguments…
#   Description:
#       If VARIABLE is not set, the function calls `skip`. Otherwise the variable is expanded and
#       used as command to run. The function is intended to be used in tests: if a required tool
#       is not installed, the test skipped.
#   Example:
#       call PANDOC -f markdown -t html ...
#
call() {
    local var cmd
    var=$1
    shift 1
    eval cmd="\$$var"
    test -n "$cmd" || skip "‘$var’ environment variable is not set"
    $cmd "$@"
};

#   ------------------------------------------------------------------------------------------------
#   Name:
#       run — show, run a command, save its output and errors, and show its output, errorts and
#       status
#   Usage:
#       run command options… arguments…
#   Description:
#       The `command` is executed, its stdout and stderr are redirected to temp files.
#
run() {
    local status=0
    say "\$ $*"
    "$@" > $tmpfile.out 2> $tmpfile.err || status=$?
    if [[ -s $tmpfile.out ]]; then
        say "Output:"
        cat $tmpfile.out
    else
        say "Output: (none)"
    fi
    if [[ -s $tmpfile.err ]]; then
        say "Errors:"
        cat $tmpfile.err
    else
        say "Errors: (none)"
    fi
    say "Status: $status"
    return $status
};

#   ------------------------------------------------------------------------------------------------
#   Name:
#       grep — grep command wrapper
#   Usage:
#       grep options… arguments…
#   Description:
#       The `grep` command exits with status 0 (true) if there are some lines in output, 1 (false)
#       if output is empty, and status > 1 if there some errors. Tests will likely check for 0 and
#       1, but status > 1 is definetely error and should be reported immediately.
#
grep() {
    # TODO: Use GREP env var.
    local status=0
    command grep "$@" || status=$?
    [[ $status -le 1 ]] || die "grep exited with status $status"
    return $status
}

#   ------------------------------------------------------------------------------------------------
#   Name:
#       egrep — egrep command wrapper
#   Usage:
#       egrep options… arguments…
#   Description:
#       See `grep` function description.
#
egrep() {
    # TODO: Use EGREP env var.
    local status=0
    command grep -E "$@" || status=$?
    [[ $status -le 1 ]] || die "grep exited with status $status"
    return $status
}

#   ------------------------------------------------------------------------------------------------
#   Name:
#       vle — compare versions
#   Usage:
#       vle lhs rhs
#   Description:
#       The functions returns true if lhs <= rhs, and false otherwise.
#
vle() {
    [[ $# == 2 ]] || or die "vle requires 2 arguments"
    local status=0
    echo "COMPARING $1 AND $2" >&2
    printf "%s\n%s\n" "$1" "$2" | sort --version-sort --check=quiet || status=$?
    [[ $status -le 1 ]] || die "sort exited with status $status"
    echo "RESULT: $status" >&2
    return $status
}

is_x_session() {
    if [[ $XDG_SESSION_TYPE == "x11" && -n $DISPLAY ]]; then
        return 0
    else
        return 1
    fi
};

is_gnome_desktop() {
    local desktop=$XDG_SESSION_DESKTOP
    if [[
        $desktop == "gnome"          ||
        $desktop == "gnome-classic"  ||
        $desktop == "gnome-xorg"     ||
        $desktop == "ubuntu"         ||
        $desktop == "ubuntu-wayland"
    ]]; then
        return 0
    else
        return 1
    fi
};

is_kde_desktop() {
    local desktop=$XDG_SESSION_DESKTOP
    if [[ $desktop == "KDE" ]]; then
        return 0
    else
        return 1
    fi
};

is_agism_enabled() {
    local state
    state=$( gnome-extensions show agism@agism.sourceforge.io | egrep -e '^ +State: ' )
    if [[ $state =~ ^\ +State:\ +ENABLED$ ]]; then
        return 0
    else
        return 1
    fi
};

# end of file #
