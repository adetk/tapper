#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: m4/v4.m4
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


#
#   Few general-purpose m4 macros.
#


#   v4_ht, v4_nl, v4_cr, v4_sp, v4_ws
#   ------------------------------------------------------------------------------------------------
#   Return quoted strings of whitespace characters: tab, newline, carriage return, space, and all
#   together.
#
m4_define([v4_ht], m4_format([[%c]], [9]))
m4_define([v4_nl], m4_format([[%c]], [10]))
m4_define([v4_cr], m4_format([[%c]], [13]))
m4_define([v4_sp], m4_format([[%c]], [32]))
m4_define([v4_ws], v4_ht[]v4_nl[]v4_cr[]v4_sp)


#   v4_x(STRING, N)
#   ------------------------------------------------------------------------------------------------
#   Returns quoted STRING repeated N times. Leading and trailing spaces are trimmed from N. If
#   N <= 0, empty string returned.
#
m4_define([v4_x],
    [m4_cond(
        [m4_bregexp([$2], [^\s*-?[0-9]+\s*$])],
        [-1],
        [m4_fatal([$0: arg 2 is not an int: $2])],
        [m4_eval([$2 <= 0])],
        [1],
        [],
        [m4_for([i], [1], [$2], [1], [[$1]])])])


#   v4_left(STRING, N)
#   ------------------------------------------------------------------------------------------------
#   Returns N characters from the left side of the STRING. Leading and trailing spaces are trimmed
#   from N. If N <= 0, empty string returned. Result is a quoted string.
#
m4_define([v4_left],
    [m4_cond(
        [m4_bregexp([$2], [^\s*-?[0-9]+\s*$])],
        [-1],
        [m4_fatal([$0: arg 2 is not an int: $2])],
        [m4_eval([$2 <= 0])],
        [1],
        [],
        [m4_format([[%.*s]], v4_trim([$2]), [$1])])])


#   v4_right(STRING, N)
#   ------------------------------------------------------------------------------------------------
#   Returns N characters from the right side of the STRING. Leading and trailing spaces are trimmed
#   from N. If N <= 0, empty string returned. Result is a quoted string.
#
#   Implementation notes:
#       *   I need a regexp to match any character. . does not match newline, so I have to use
#           \(\s\|\S\).
#       *   Construct \{N\} does not work, so I have to repeat \(\s\|\S\) many times.
#
m4_define([v4_right],
    [m4_cond(
        [m4_bregexp([$2], [^\s*-?[0-9]+\s*$])],
        [-1],
        [m4_fatal([$0: arg 2 is not an int: $2])],
        [m4_eval([$2 <= 0])],
        [1],
        [],
        [m4_eval([$2>=]m4_len([$1]))],
        [1],
        [[$1]],
        [m4_bregexp([$1 THiS iS THe veRy eND], [\(]v4_x([\(\s\|\S\)], [$2])[\) THiS iS THe veRy eND$], [[\1]])])])


#   v4_ltrim(STRING)
#   ------------------------------------------------------------------------------------------------
#   Trims leading whitespaces from STRING. Non-leading whitespace remains intact. Returns quoted
#   string.
#
#   Implementation notes:
#       *   "^" matches to the beginning of every line.
#       *   "\`" matches to the beginning of the string/text.
#
m4_define([_v4_ltrim], [m4_bpatsubst([[$1]], [\`\(.\)\s*], [\1])])
m4_define([v4_ltrim],
    [m4_cond(
        m4_eval([$# > 1]),
        [1],
        [m4_fatal([$0: too many arguments])],
        [_v4_ltrim([$1])])])


#   v4_rtrim(STRING)
#   ------------------------------------------------------------------------------------------------
#   Trims trailing whitespaces from STRING. Non-trailing whitespace remains intact. Returns quoted
#   string.
#
#   Implementation notes:
#       *   "$" matches to the end of every line.
#       *   "\'" matches to the end of the string/text.
#
m4_define([_v4_rtrim], [m4_bpatsubst([[$1]], [\s*\(.\)\'], [\1])])
m4_define([v4_rtrim],
    [m4_cond(
        m4_eval([$# > 1]),
        [1],
        [m4_fatal([$0: too many arguments])],
        [_v4_rtrim([$1])])])


#   v4_trim(STRING)
#   ------------------------------------------------------------------------------------------------
#   Trims leading and trailing whitespaces from STRING. Inner whitespace remains intact.
#   Returns quoted string.
#
m4_define([_v4_trim], [_v4_ltrim(_v4_rtrim([$1]))])
m4_define([v4_trim],
    [m4_cond(
        m4_eval([$# > 1]),
        [1],
        [m4_fatal([$0: too many arguments])],
        [_v4_trim([$1])])])


#   v4_atrim(STRING, STRING, ...)
#   ------------------------------------------------------------------------------------------------
#   Trims leading and trailing whitespaces from each STRING. Inner whitespace remains intact.
#   Returns comma-separated list of quoted strings. v4_atrim is convenient to preprocess macro
#   arguments, e. g.:
#
#       m4_define([BC_PATH_PROGS], [AC_PATH_PROGS(v4_atrim($@))])
#
#   So ypu can think "a" stands for "arguments" (or "all").
#
m4_define([v4_atrim], [m4_map_args_sep([_v4_trim(], [)], [,], $@)])


#   v4_include(FILENAME)
#   ------------------------------------------------------------------------------------------------
#   Like m4_include, but file content is included as quoted string.
#
m4_define([_v4_include], [m4_esyscmd([echo -ne '['; cat "$1"; rc=$?; echo -ne ']'; exit $rc])])
m4_define([v4_include],
    [m4_ignore(
    )m4_cond(
        m4_eval([$# > 1]),
        [1],
        [m4_fatal([$0: too many arguments])])m4_ignore(
    )_v4_include([$1])m4_ignore(
    )m4_cond(
        [m4_sysval],
        [0],
        [m4_ignore([everything is ok])],
        [m4_fatal([$0: shell command exited with status ]m4_sysval)])m4_ignore(
    )])


# TODO: Write a macro to strip trailing empty elements from a list. "Empty" means whitespace and
# comments-only.


# end of file #
