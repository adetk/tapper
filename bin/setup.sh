#!/bin/bash

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/setup.sh
#
#   Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
#
#   This file is part of Tapper.
#
#   Copying and distribution of this file, with or without modification, are permitted in any
#   medium without royalty provided the copyright notice and this notice are preserved. This file
#   is offered as-is, without any warranty.
#
#   SPDX-License-Identifier: FSFAP
#
#   ---------------------------------------------------------------------- copyright and license ---

set -eo pipefail

#   ------------------------------------------------------------------------------------------------
#   Functions
#   ------------------------------------------------------------------------------------------------

# Usage: help
# Prints help message.
function help() {
    #    123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
    printf "%s\n" \
        "Usage: setup.sh [OPTION]…"                                                       \
        "Install the packages needed to build and test Tapper."                           \
        "Options:"                                                                        \
        "    -a      Install all packages, including development ones. By default,"       \
        "            packages required for Tapper development are not installed."         \
        "    -f ID   Do not detect current distro but use the specified distro id."       \
        "    -h      Print this help and exit."                                           \
        "    -n      Dry run: show command(s) but do not run them. Implies -y."           \
        "    -y      Do not ask questions, install packages non-interactively, if"        \
        "            possible."                                                           \
        "Examples:"                                                                       \
        "    Inspect what will be installed and how:"                                     \
        "        $ setup.sh -n -y"                                                        \
        "    Install the packages:"                                                       \
        "        $ sudo setup.sh"

};

function say() {
    printf "%-20s: %s\n" "$@"
};

# Usage: die message…
# Prints message(s) (message per line) to stderr and exits with status `$status` (255 by default).
function die() {
    local msg
    for msg in "$@"; do
        printf "%s: %s\n" "$0" "$msg" >&2
    done
    exit ${status:-255}
};

yesstr=            # Positive answer, 'y' by default.
yesexpr=           # Regular expression to detect positive answer, '^[yY]' by default.
nostr=             # Negative answer, 'n' by default.

# Usage: ask question
# Asks a question. Returns true in case of positive answer, and false in case of negative.
# Locale data used if possible.
function ask() {
    if [[ -n $yes ]]; then
        return 0    # yes
    fi
    if [[ "$yesstr$yesexpr$nostr" == "" ]]; then
        # yesstr, yesexpr, nostr are not yet initialized.
        # Try to fetch them from current locale:
        yesstr=$( locale yesstr )
        yesexpr=$( locale yesexpr )
        nostr=$( locale nostr )
        # On Mint `locale yesstr` returns empty string! `locale nostr` does the same.
        # So, use these variables only if they are non-empty:
        if [[ -n $yesstr && -n $yesexpr && -n $nostr ]]; then
            yesstr=${yesstr:0:1}
            nostr=${nostr:0:1}
        else
            # Can't get them from locale, use default values in such a case:
            yesstr='y'
            yesexpr='^[yY]'
            nostr='n'
        fi
    fi
    local answer
    read -ep "$1 [${yesstr^}/${nostr,}] " answer
    if [[ ${answer:-${yesstr}} =~ $yesexpr ]]; then
        return 0 # yes
    else
        return 1 # no
    fi
};

# Usage: is_in item elem…
# Returms true, if item is in the list of elements, and false otherwise.
function is_in() {
    local item=$1
    shift 1
    local it
    for it in "$@"; do
        if [[ $item == $it ]]; then
            return 0
        fi
    done
    return 1
};

packages=()

# Usage: add_packages pkg…
# Adds package(s) to the array `$packages`, if the package is not yet added to the array.
function add_packages() {
    local pkg
    for pkg in "$@"; do
        if ! is_in "$pkg" "${packages[@]}"; then
            packages+=( "$pkg" )
        fi
    done
};

# Usage: run [command args…]
# Shows the command, and runs it (if `$dry` is not set). If command is not specified, does nothing.
function run() {
    if [[ $# -gt 0 ]]; then
        # Quote command words, so printed command may be copied and executed by the user as is.
        local command=()
        local arg
        for arg in "$@"; do
            printf -v arg "%q" "$arg"
            command+=( "$arg" )
        done
        say "Command to run" "${command[*]}"
        if [[ -z $dry ]]; then
            "$@"
        fi
    fi
};

# Usage: prologue
# Default prologue (actions to execute before installing the packages).
function prologue() {
    : # Do nothing.
};

#   ------------------------------------------------------------------------------------------------
#   Script
#   ------------------------------------------------------------------------------------------------

#
#   Parse command line.
#
status=2        # Exit with status 2 in case of command-line error.
all=            # If set, install all packages, incl. development packages.
dry=            # If set, show commands but do not execute them.
id=             # If set, do not detect current distro but use the specified id.
yes=            #If set, perform non-interactive installation.
while getopts ":ahf:ny" opt "$@"; do
    case $opt in
        ( a )   all=yes;;
        ( h )   help; exit 0;;
        ( f )   id=$OPTARG;;
        ( n )   dry=yes; yes=yes;;
        ( y )   yes=yes;;
        ( : )   die "Option '-$OPTARG' requires an argument";;
        ( ? )   die "Invalid option '-$OPTARG' (use '-h' option to print help)";;
    esac
done
shift $(( OPTIND - 1 ))
[[ $# -eq 0 ]] || die "Too many arguments"
status=3        # Exit with status 3 if error is caused in run time.

#
#   Detect current distro (or use specified one).
#
if [[ -n $id ]]; then
    distros=( "$id" )
    say "Forced distro" "$id"
else
    # Detect current distro id(s).
    release=/etc/os-release
    [[ -f $release ]] || die "No '$release' file found"
    . "$release"
    [[ -n $ID ]] || die "No ID variable found in '$release' file."
    distros=( $ID $ID_LIKE )
    say "Detected distro" "${distros[0]}"
fi

install=()      # Command to install packages, with options, if any.
for (( i = 0; i < ${#distros[@]}; i = i + 1 )); do
    distro=${distros[i]}
    case $distro in
        ( arch )
            # Arch Wiki discourages using `pacman -Sy`, they recommend to use `pacman -Syu` instead.
            # But I am not going to update entire user system.
            function prologue() {
                say "*Note*" "Arch is a rolling distro. You may need to update it first with "
                say "*Note*" "    pacman -Syu"
                say "*Note*" "before installing packages."
            };
            install=( pacman -S --needed ${yes:+--noconfirm} )
            break;;
        ( debian | altlinux )
            #   Alt:
            #       `ID_LIKE` is not set, but Alt uses `apt-get` like Debian. Requires
            #       `apt-get update`.
            #   Debian:
            #       Does not require `apt-get update`.
            #   Trisquel:
            #       `ID_LIKE=debian`, but requires `apt-get update`.
            function prologue() {
                run apt-get update ${yes:+-y}
            };
            install=( apt-get install ${yes:+-y} )
            break;;
        ( fedora | mageia | openmandriva )
            install=( dnf install ${yes:+-y} )
            break;;
        ( opensuse )
            install=( zypper install ${yes:+-y} )
            break;;
        ( ubuntu )
            #   Mint:
            #       `ID_LIKE` is "ubuntu debian". Without `apt-get update` fails with error:
            #           Failed to fetch http://archive.ubuntu.com/
            #       `apt-get update` fails with error:
            #           The repository 'cdrom://Linux Mint 21 _Vanessa_ - Release amd64 20220726
            #           jammy Release' does not have a Release file.
            #   Ubuntu:
            #       `ID_LIKE` is "debian", but Debian case is not suitable: Ubuntu requires adding
            #       the repository 'unverse', otherwise some packages will not be found.
            function prologue() {
                echo "Need to enable repository 'universe'."
                if ask "Do you want to enable it?"; then
                    run add-apt-repository ${yes:+-y} universe
                fi
                run apt-get update ${yes:+-y}    # TODO: Do I need it?
            };
            install=( apt-get install ${yes:+-y} )
            break;;
        ( void )
            function prologue() {
                run xbps-install ${yes:+-y} -S
            };
            install=( xbps-install ${yes:+-y} )
            break;;
        ( * )
            # I do not know the package manager for this distro, but let's try the next distro id.
            # Such approach works for many distros that properly set `ID_LIKE` variable in
            # their `/etc/os-release`.
            if [[ $i < ${#distros[@]} ]]; then
                say "*Note*" "Do not know how to install packages in the '$distro' distro,"
                say "*Note*" "but it is known that this distro is like '${distros[i+1]}' distro."
            fi;;
    esac
done
[[ ${#install[@]} -gt 0 ]] || die "Do not know how to install packages."
say "Package manager" "${install[0]}"

#
#   Detect list of packages to install.
#
for distro in "${distros[@]}"; do
    setup=$( dirname "$0" )/setup-$distro.sh
    if [[ -f $setup ]]; then
        say "List of packages" "$setup"
        bld_packages=()
        dev_packages=()
        . "$setup"
        packages=()
        add_packages "${bld_packages[@]}"
        if [[ -n $all ]]; then
            add_packages "${dev_packages[@]}"
        fi
        say "Packages to install" "${packages[*]}"
        break
    fi
    setup=
done
[[ -n $setup ]] || die "List of packages to install not found."

#
#   Install the packages.
#
ask "Proceed?"
prologue
run "${install[@]}" "${packages[@]}"

# end of file #
