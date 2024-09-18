<!--
    ---------------------------------------------------------------------- copyright and license ---
    File: doc/en/bugs.md
    Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
    This file is part of Tapper.
    SPDX-License-Identifier: GPL-3.0-or-later
    ---------------------------------------------------------------------- copyright and license ---
-->

BUGS
====

If you are a newbie, read Simon Tatham’s essay “[How to Report Bugs Effectively][]”. The essay is
available in several languages.

[How to Report Bugs Effectively]: https://www.chiark.greenend.org.uk/~sgtatham/bugs.html
[Как эффективно сообщать об ошибках]: https://www.chiark.greenend.org.uk/~sgtatham/bugs-ru.html

A bug report or help request must include:

1.  Brief description of your system. Run the following commands and show me their exact output:

        $ cat /etc/os-release
        $ uname -m

2.  How you installed Tapper:

    a.  If you downloaded a prebuilt RPM from Tapper’s Copr repository: Provide the exact name of
        the RPM package: `rpm -q tapper`.
    b.  If you built a binary RPM from source RPM: Provide the exact name of the source RPM
        package.
    c.  If you built Tapper from sources: Provide the exact name of the source tarball.

3.  In case of build problem: Exact build commands and their exact output.

4.  In case of run time problem: Run the following commands and show me their exact output (omit
    the third command in case of non-GNOME desktop (Ubuntu is the GNOME desktop)):

        $ echo $XDG_SESSION_TYPE
        $ echo $XDG_SESSION_DESKTOP
        $ gnome-extensions show agism@agism.sourceforge.io
        $ gsettings list-recursively io.sourceforge.kbd-tapper

    Then run Tapper from the terminal and show me exact command you type and its exact output;
    also describe your actions, the actual Tapper behavior, and your expectations.

Collect all the requested information and send the bug report to <@PACKAGE_EMAIL@>.

<!-- end of file -->
