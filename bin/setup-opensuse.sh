#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/setup-opensuse.sh
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

# This file works for both opensuse-leap and opensuse-tumbleweed.

# Packages for building:
bld_packages=(
    # Mandatory tools:
        gcc-c++
        make
        perl-base
        pkgconf
    # Mandatory Perl modules:
        'perl(open)'
        'perl(strict)'
        'perl(utf8)'
        'perl(warnings)'
        'perl(Getopt::Long)'
        'perl(Path::Tiny)'
        'perl(Pod::Usage)'
        'perl(Try::Tiny)'
    # Optional libraries:       # Required by:
        'pkgconfig(glibmm-2.4)'     # GNOME and KDE layouters, settings.
        'pkgconfig(giomm-2.4)'      # GNOME and KDE layouters, settings.
        'pkgconfig(libcap)'         # Tapper to drop privileges.
        'pkgconfig(libevdev)'       # libevdev emitter.
        'pkgconfig(libinput)'       # libinput listener.
        'pkgconfig(libudev)'        # libinput listener.
        'pkgconfig(x11)'            # X Recors listener and Xkb layouter.
        'pkgconfig(xtst)'           # X Test emitter.
    # Optional build tools:     # Required by:
        libcap-progs                # tapper if libcap is used.
        m4                          # Man page.
        pandoc                      # Man page, metainfo file.
        rpm-build                   # RPM package.
    # Optional Perl modules:    # Required by:
        'perl(HTML::Entities)'      # Metainfo file.
        'perl(JSON)'                # Metainfo file.
    # Optional test tools:      # Required by:
        AppStream                   # RPM package, metainfo file.
        desktop-file-utils          # RPM package, desktop file.
)

# Packages for development:
dev_packages=(
    # Optional build tools:     # Required by:
        doxygen                     # Source documentation.
        m4                          # HTML pages.
        pandoc                      # HTML pages.
        wget                        # HTML pages.
    # Optional test tools:      # Required by:
        aspell                      # Man page, HTML pages.
        aspell-en                   # Man page, HTML pages.
        cppcheck                    # Source.
        man                         # Man page.
        rpmlint                     # RPM package.
        tidy                        # HTML pages.
)

# end of file #
