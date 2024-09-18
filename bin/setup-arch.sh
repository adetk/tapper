#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/setup-arch.sh
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
        gcc
        make
        perl
        pkgconf
    # Mandatory Perl modules:
        perl-path-tiny
        perl-try-tiny
    # Optional libraries:       # Required by:
        glibmm                      # GNOME and KDE layouters, settings.
        libcap                      # Tapper to drop privileges.
        libevdev                    # libevdev emitter.
        libinput                    # libinput listener.
        systemd-libs                # libinput listener (the package contains libudev).
        libx11                      # X Recors listener, Xkb layouter.
        libxtst                     # X Test emitter.
    # Optional build tools:
        m4                          # Man page.
        pandoc                      # Man page, metainfo file.
    # Optional Perl modules:    # Required by:
        perl-html-parser            # Metainfo file.
        perl-json                   # Metainfo file.
    # Optional test tools:      # Required by:
        appstream                   # Metainfo file.
        desktop-file-utils          # Desktop file.
)

# Packages for Tapper development:
dev_packages=(
    # Optional build tools:     # Required by:
        doxygen                     # Source documentation.
        m4                          # Man page, HTML pages.
        pandoc                      # Man page, HTML pages.
        wget                        # HTML pages.
    # Optional test tools:
        aspell                      # Man page, HTML pages.
        aspell-en                   # Man page, HTML pages.
        cppcheck                    # Source.
        man-db                      # Man page.
        tidy                        # HTML pages.
)

# end of file #
