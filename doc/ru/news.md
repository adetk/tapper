<!--
    ---------------------------------------------------------------------- copyright and license ---
    File: doc/en/news.md
    Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
    This file is part of Tapper.
    SPDX-License-Identifier: FSFAP
    ---------------------------------------------------------------------- copyright and license ---
-->

NEWS
====

This document describes *user-visible changes* in Tapper.

0.7.4 @ 2023-02-01
------------------

Bug fix: `xdotool type 🫠` caused Tapper's "Out of range [9…255]" error. Thanks to emorozov for
reporting.

0.7.3 @ 2022-12-31
------------------

The GNOME layouter monitors the GNOME session. If the session becomes inactive (e. g. when the user
switches to the GNOME Display Manager screen or to another session), Tapper switches to the passive
mode: it continues to listen user input events, but does not execute any actions (does not activate
layouts and does not emulate keystrokes). If the session becomes active, Tapper switches back to
normal mode. This prevents unwanted interference between sessions.

Tapper was unable to detect Ubuntu desktop running on top of a Wayland session. Thanks to Михаил
Желудков for reporting the problem. Now Tapper in such case (if `XDG_SESSION_DESKTOP` is
`ubuntu-wayland`) selects the GNOME layouter.

If the `--show-taps` option specified, Tapper shows taps, but ignores all the assignments (i. e.
does not execute any actions: does not activate keyboard layouts and does not emulate keystrokes).

Man page is available in [Russian language][man.ru].

0.7.2 @ 2022-12-17
------------------

The `setup.sh` script works for Void Linux (glibc is required, building with musl will fail).

Bug fix: Russian home page contained incorrect installation instruction for openSUSE. Thanks to
KOT040188 for reporting.

0.7.1 @ 2022-12-15
------------------

Tapper privileges are cut down: Tapper does not need full `root` power, two capabilities
`cap_setuid` and `cap_setgid` are enough. Instead of `chmod u+s` the installer performs `setcap
cap_setuid,cap_setgid=p`. Tapper drops all capabilities in the very beginning.

License for small supporting files changed to FSFAP.

Metainfo file is added.

The `setup.sh` script works for AltLinux (`configure && make` works in AltLinux, but building an
RPM package from the provided spec file does not).

0.7.0 @ 2022-10-01
------------------

**Highlight:** The KDE layouter is added, it works for both X Window System and Wayland. There is a
minor drawback, though: the KDE layouter cannot ring the bell.

**Breaking change:** Because of the KDE layouter addition, GSettings schema is changed. Check
the layouter selection setting after updating Tapper.

Error reporting is improved in case if no suitable listener/layouter/emitter is found.

If layouter is not explicitly selected and the `XDG_SESSION_DESKTOP` environment variable is
`gnome-xorg`, Tapper uses the GNOME layouter.

`BUGS.md` documents what information must be included into a bug report.

Tapper desktop support matrix is added to the Tapper home page.

Tapper RPM requires Agism RPM only if `gnome-shell` package is already installed. Also Tapper RPM
recommends `dconf-editor` RPM.

0.6.3 @ 2022-06-14
------------------

Bug fix: Syntax of action key codes in settings and in the command line was inconsistent: codes in
settings required a `#` prefix, while key codes in the command line did not. Now key codes are
written with no prefix, as documented.

The `--autostart` and `--no-autostart` options respect the `--datadir` configuration option.

The `--autostart` and `--no-autostart` options respect the `XDG_CONFIG_HOME` environment variable.

Spaces are tolerated in actions (e. g. `KEY_CAPSLOCK = @1, 1`) in both settings and command-line
arguments.

0.6.2 @ 2022-06-04
------------------

The `--dconf-editor` option is introduced to edit Tapper settings with the `dconf-editor` graphical
tool.

The `--autostart` option is introduced to set up Tapper to start automatically when you log in. The
`--no-autostart` option disables autostart.

The libinput listener respects the `XDG_SEAT` environment variable if it is set.

Bug fix: The GNOME layouter ringed the bell regardless of the `--bell`/`--no-bell` option and
settings. Thanks to papin-aziat and Vitaly for reporting.

0.6.1 @ 2022-06-02
------------------

RHEL 8 build is fixed.

Minor changes in the documentation (HTML pages and man page).

0.6.0 @ 2022-06-01
------------------

**Highlight:** Tapper works in GNOME 41 and does not require “Unsafe Mode”.

**Important:** The GNOME layouter requires [Agism], a GNOME Shell extension, which should be
installed and enabled. See “[Caveats/GNOME layouter does not work]” in [tapper(1)].

**Breaking change:** Since Tapper can’t hide input events from other programs, “interceptor” is
renamed to “listener”: the `--interceptor` command line option is renamed to `--listener`, key in
GSettings schema is also renamed.

**Highlight:** Tapper can emulate keystrokes. See “[Description/Actions]”,
“[Description/Assignments]” and “[Examples/Advanced usage]” in [tapper(1)].

**Breaking change:** Because of ability to emulate keystrokes, the assignment syntax is changed.
Keyboard layouts must be prefixed with `@` in order to distinguish between keyboard layouts and
keys. Both command line and GSettings schema are affected.

Tapper drops unnecessary privileges as soon as possible. See “[Description/Security note]” in
[tapper(1)].

Build: libjson-glib is not required any more. libevdev is required.

Build: `setup.sh` script are added to install required packages, it works for Arch, Debian, Fedora,
Mageia, openSUSE, and Ubuntu.

0.5.0 @ 2021-01-03
------------------

**Highlight:** Tapper is tested in bunch of desktops: Cinnamon, GNOME Classic, KDE, LXDE, LXQt,
Mate, Xfce. Tappers works perfectly in all these desktops, so “GNOME Shell” is dropped from Tapper
headline. Now it is “a keyboard layout switcher for X Window System and Wayland”. GNOME Shell is
still a primary target for Tapper though.

Bug fix: Command line assignments didn’t override settings assignments.

GNOME detection is changed: The `XDG_CURRENT_DESKTOP` environment variable is retired, the
`XDG_SESSION_DESKTOP` is analyzed instead.

Desktop file is updated so Tapper is always shown (not just in GNOME, as before).

Tapper home page is updated with getting instructions for Arch, Mageia and openSUSE.

The `--no-default-assignments` command line option is introduced. It is used for testing purposes
and has little value for users.

Few corner cases are handled more gracefully:

*   The `--interceptor=auto` command line option now selects XRecord interceptor regardless of
    `XDG_SESSION_TYPE` environment variable if XRecord is the only available interceptor (in other
    words, if libinput interceptor is disabled at build time). Earlier Tapper in such a condition
    fired assertion failure and aborted with core dump.

*   Similarly, the `--layouter=auto` command line option selects GNOME layouter regardless of
    `XDG_SESSION_DESKTOP` environment variable if GNOME is the only available layouter. Earlier
    Tapper in such a condition fired assertion failure and aborted with core dump.

*   Similarly, if Tapper is build with no interceptors or no layouters (useless but possible
    configuration), default assignments are not used to avoid error caused by bad arguments. Such
    error can confuse user since no arguments specified in command line.

`INSTALL.md` now includes build instructions for Mageia and openSUSE.

Build: `configure` options `--enable-libinput` and `--enable-x` are renamed to `--with-libinput`
and `--with-x` respectively to follow Autoconf guidelines.

Build: New configure options `--with-glib` and `--without-glib` are introduced to decouple GLib
usage from GNOME support. Now it is possible to disable GNOME layouter but still use GSettings to
store configuration.

Build: The `--enable-author-testing` configure option is introduced. If author testing is disabled,
some non-functional tests (e. g. cppcheck, spell check) are not run because there is no point in
running these tests for packagers and end users.

Tests are reworked:

*   Tapper configuration is respected now. For example, if Tapper is configured with the
    `--without-x` option, tests skip all checks that require the XRecord interceptor and/or the Xkb
    layouter.

*   Execution environment is respected now. For example, if tests are run in console session, tests
    requiring X Window System are skipped.

0.4.4 @ 2020-05-06
------------------

The `--syslog` command line option is introduced.

0.4.3 @ 2020-03-10
------------------

Build: Empty `%{dist}` rpm macro confused make. This affected build on Arch if `rpm-build` is
available. Thanks to Jozef Riha for reporting.

Bug fix: If keyboard layout name contained Unicode characters, Tapper could complain about bad JSON
string (e. g. “unterminated string constant” or “invalid byte sequence in conversion input”).
Thanks to Jozef Riha for reporting.

Bug fix: The last layout in the intro message was printed with no layout name. Thanks to Jozef Riha
for reporting.

Build: `configure`/`make` failed on Ubuntu (unresolved references to the `glibmm` library).

`INSTALL.md`: Added (incomplete) instructions for Arch and Debian/Ubuntu.

Site: Added a link to project page @ SourceForge.

0.4.2 @ 2019-08-14
------------------

Site: Site search is added to the *Site map* page.

0.4.1 @ 2019-08-06
------------------

Tapper intercepts mouse buttons as well. It prevents interpreting key+click as key tap.

0.4.0 @ 2019-07-29
------------------

**Highlight:** Tapper works in Wayland (see `--interceptor=libinput`).

Notion of “interceptor” is introduced. The `--interceptor` option is added. There are 3
interceptors: the `xrecord` is the only interceptor available in the previous release, it works in
X Window System only; the new `libinput` interceptor works in both Wayland and X; `auto` is not a
real interceptor but instruction to select suitable interceptor automatically.

Notion of “layouter” is introduced. The `--layouter` option is added. There are 3 layouters: the
`gnome-shell` layouter works nice in GNOME; the `xkb` layouter may work in non-Gnome; `auto` is not
a real layouter but instruction to select suitable layouter automatically.

Tapper now reads configuration from GSettings/dconf. The `--load-settings` and `--save-settings`
options are added.

The `--show-taps` option is added.

Help message and manual page are updated.

Desktop file is added, so Tapper can be started from GNOME Shell and added to *Startup
Applications* using *GNOME Tweaks* application.

Mouse event interception is not implemented yet.

**Breaking change:** By default Tapper assigns the left and right `Ctrl`{.k}s (not `Shift`{.k}s, as
before).

0.3.1 @ 2017-03-10
------------------

Tapper continues to work if GNOME Shell restarts.

0.3.0 @ 2017-01-19
------------------

**Highlight:** GNOME mode is returned. In GNOME mode, number of layouts is not limited; GNOME
keyboard layout indicator is properly updated.

0.2.2 @ 2016-12-02
------------------

Size of executable file is significantly reduced.

Minor changes in documentation and HTML pages.

0.2.1 @ 2016-01-31
------------------

Help message is updated.

Man page is added.

HTML pages are updated.

0.2 @ 2016-01-25
----------------

GNOME support is stripped down — it does not work in GNOME 3.18 anyway.

Tapper intercepts mouse button press/release events to avoid switching keyboard layout on
`Ctrl`+Click (or `Shift`+Click).

Tapper returns non-zero code if anything goes wrong.

Bug fix: Tapper used not-yet-initialized context.

0.1 @ 2014-07-26
----------------

Initial release.

<!-- end of file -->
