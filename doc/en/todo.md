<!--
    ---------------------------------------------------------------------- copyright and license ---
    File: doc/en/todo.md
    Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
    This file is part of Tapper.
    SPDX-License-Identifier: GPL-3.0-or-later
    ---------------------------------------------------------------------- copyright and license ---
-->

TODO
====

*   Libinput mouse buttons do not depend on mouse settings (left- or right-handed mouse), while
    XRecord mouse buttons do. xrecord listener should work like libinput listener and always
    report left mouse button as left mouse botton.

*   Get back "Why?" section.

*   How to test tapper settings?

*   Calculate Id from Home URL?

*   Use std::optional (C++17)?

*   Try to split out few libs, e. g. layouter-x, layouter-gnome, listener-x, listener-li,
    and load only required libs.

*   configure: Make sure make is GNU Make.

*   configure: make check if -std=c++11 supported.

*   Exit status — looks like it is not always correct (2 and 3).

*   Retire X key names — let always use Linux event names. I do use universal key codes, so why
    don't I use universal key names?

*   Complete KDE layouter: ring the bell.

*   Try to build RPM on AltLinux? (They use `rpm` instead of `rpm-build`; `rpm` does not accept
    the spec file with no reason why.)

*   Fix build on Astra? (settings.cpp:261:23: error: class Gio::Settings has no member names
    set_enum).

*   Since the GNOME layouter monitors signals from the GNOME Session, may be it could monitor
    signal from GSettings to detect new value of autorepeat delay?

*   Limit `--show-taps` with 10 taps or 10 seconds?

*   Move session monitoring into a dedicated component?

<!-- end of file -->
