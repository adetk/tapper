<!--
    ---------------------------------------------------------------------- copyright and license ---
    File: doc/en/install.md
    Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
    This file is part of Tapper.
    SPDX-License-Identifier: GPL-3.0-or-later
    ---------------------------------------------------------------------- copyright and license ---
-->

INSTALL
=======

This document describes building and installing Tapper *from sources*.

Unpacking
---------

Unpack the source tarball first:

    $ tar xzf tapper-@ver@-@rel@.tar.gz
    $ cd tapper-@ver@-@rel@

<!-- Replace @ver@-@rel@ with the actual version and release of the Tapper tarball. -->

In-source build is prohibited, so you have to create a build directory and change to it before
running `configure`:

    $ mkdir _build
    $ cd _build

All the intermediate and output files (including HTML pages and RPM packages) will be located in
the build directory.

Installing build and test tools
-------------------------------

Tapper provides the `setup.sh` script to install the tools and libraries needed to build Tapper:
the program, the man page, and the RPM package (in case of RPM-based distro). The script works for
Alt, Arch, Debian, Fedora, Mageia, OpenMandriva, openSUSE, Ubuntu, and Void. (The script will
likely work for other distros as well, but I can not guarantee it — I can't test dozens of
GNU/Linux distros.)

In case of any doubt use the `-n` option for a "dry" run — the script will show the commands, but
will not actually run them, e. g.:

    $ ../bin/setup.sh -n
    Detected distro     : ubuntu                                                             ❶
    Package manager     : apt-get                                                            ❷
    List of packages    : ../bin/setup-ubuntu.sh                                             ❸
    Packages to install : g++ make perl-base pkgconf libpath-tiny-perl libtry-tiny-perl …    ❹
    Command to run      : add-apt-repository universe -y                                     ❺
    Command to run      : apt-get update -y                                                  ❺
    Command to run      : apt-get install -y g++ make perl-base pkgconf libpath-tiny-perl …  ❺

Note that a dry run does *not* require superuser privileges. Check the output of the script: make
sure your distro is detected correctly (❶), verify the choice of package manager (❷) and the list
of packages to install (❹). If everything is ok, run the script with superuser privileges for
actual installation:

    $ sudo ../bin/setup.sh

Use the `-y` option to install packages non-interactively, without questions.

If `sudo` command is not available in your distro (e. g. in Mageia), use `su` command instead, for
example:

    $ su -c "../bin/setup.sh -y"

If you do not want to install some packages, review the ‘list of packages’ file (❸) and comment out
unwanted packages. Do not comment out mandatory packages — `configure` will fail, but optional
packages may be commented freely. However, lack of optional build tools may disable Tapper
components: e. g. if `pandoc` is missed, the man page will not be built. Lack of optional test
tools will cause some tests to skip. If you are not going to develop Tapper, most of the test tools
can be safely commented out. (Stuff is a bit more complicated, though: for example, building the
RPM package does require some test tools.)

If you do not like an idea of running `setup.sh` with superuser privileges, you can run ‘Commands
to run’ (❺) manually.

Configuring
-----------

Run `configure` in the build directory:

    $ ../configure

If you are going to install Tapper locally to your home directory, run:

    $ ../configure --prefix=$HOME/.local

Run `configure` with  `--help` option to see list of all available options.

### Options to disable Tapper functionality and components

`--disable-gnome`

:   Disable GNOME support (namely, GNOME layouter).

`--disable-kde`

:   Disable KDE support (namely, KDE layouter).

`--disable-metainfo`

:   Do not build metainfo file. Useful if your distro does not provide `pandoc` program.

`--disable-man`

:   Do not build man page. Useful if your distro does not provide `pandoc` program.

`--disable-rpm`

:   Do not build RPM packages. Useful in case of non-RPM distros (e. g. Arch, Debian, Ubuntu, etc).

`--disable-html`

:   Do not build HTML pages.

### Options to disable libraries

`--without-glib`

:   Do not use GLib libraries. These libraries are required for GNOME and KDE layouters and
    for loading and saving settings.

`--without-libevdev`

:   Do not use libevdev library. This library is required for the libevdev emitter.

`--without-libinput`

:   Do not use libinput library. This library is required for the libinput listener.

`--without-x`

:   Do not use X Window System libraries. These libraries are required for the XRecord listener and
    the Xkb layouter.

Building
--------

To build Tapper, just run `make`:

    $ make

By default, RPM packages and HTML pages are not built. To build them, specify targets `rpm` and/or
`html` in the command line:

    $ make rpm html

All GNU Automake standard targets (`check`, `dist`, etc) are supported. There are some non-standard
convenience targets, run `make help` to see them.

Testing
-------

To execute tests, run

    $ make check

Note: In contrast to `make all` that does not build RPM packages and HTML pages, `make check` tests
really all components, including optional ones.

Installing
----------

I strongly recommend building and installing the package appropriate for your GNU/Linux distro, if
it is possible.

### Installing the package

**Fedora**

    $ DIST=$(rpmbuild --eval %{?dist})
    $ sudo dnf install ./tapper-@ver@-@rel@$DIST.x86_64.rpm

<!-- Replace @ver@ and @rel@ with the version and release of the actual RPM. -->

**Mageia**

    $ DIST=$(rpmbuild --eval %{?dist})
    $ su -c "dnf install ./tapper-@ver@-@rel@$DIST.x86_64.rpm"

**OpenMandriva**

    $ DIST=$(rpmbuild --eval %{?disttag:-%{disttag}})
    $ sudo dnf install ./tapper-@ver@-@rel@$DIST.x86_64.rpm

**openSUSE**

    $ sudo zypper install ./tapper-@ver@-@rel@.x86_64.rpm

<!-- Replace @ver@ and @rel@ with the version and release of the actual RPM. -->

### Installing the program

If installing the package is not feasible, run

    $ sudo make install

If you want to strip debug information from the installed binary, run:

    $ sudo make install-strip

(Use `su -c "..."` instead of `sudo` if `sudo` is not available in your distro.)

If you configured Tapper for local install, run:

    $ make install CHGRP=: CHOWN=: SETCAP=:

Such local non-privileged installation will work for all desktops running on top of X Window System
session; attempts to use the libinput listener or the libevdev emitter (e. g. to run Tapper in
Wayland session) will fail, though.

<!-- end of file -->
