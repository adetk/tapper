<!--
    ---------------------------------------------------------------------- copyright and license ---
    File: html/index.md
    Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
    This file is part of Tapper.
    SPDX-License-Identifier: GPL-3.0-or-later
    ---------------------------------------------------------------------- copyright and license ---
-->

m4_dnl Unfortunately, YAML metadata block doesn't work: `header-includes` is overridden by
m4_dnl command-line options `-H` and `-V header-includes=...`.
---
header-includes:    '<meta http-equiv="refresh" content="0;URL=en.html"/>'
...

m4_dnl If redirection does not work, show the map page:
m4_include({{map.md}})

<!-- end of file -->
