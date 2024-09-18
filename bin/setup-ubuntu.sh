#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/setup-debian.sh
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

# Packages for building:
bld_packages=(
    # Mandatory tools:
        g++
        make
        perl-base
        # Debian provides both, `pkgconf` and `pkg-config` packages. They do the same.
        # Use only one of them, not both:
            pkgconf
            #~ pkg-config
    # Mandatory Perl modules:
        libpath-tiny-perl
        libtry-tiny-perl
    # Optional libraries:       # Required by:
        libglibmm-2.4-dev           # GNOME and KDE layouters, settings.
        libcap-dev                  # Tapper to drop privileges.
        libevdev-dev                # libevdev emitter.
        libinput-dev                # libinput listener.
        libudev-dev                 # libinput listener.
        libx11-dev                  # X Record listener, Xkb layouter.
        libxtst-dev                 # X Test emitter.
    # Optional build tools:
        m4                          # Man page.
        pandoc                      # Man page, metainfo file.
    # Optional Perl modules:    # Required by:
        libjson-perl                # metainfo file.
    # Optional test tools:      # Required by:
        appstream                   # Metainfo file.
        desktop-file-utils          # Desktop file.
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
        man-db                      # Man page.
        tidy                        # HTML pages.
)

# end of file #
