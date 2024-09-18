#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/setup-void.sh
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
        gcc                     # In Void, `g++` is in `gcc` package.
        make
        perl
        pkgconf
    # Mandatory Perl modules:
        perl-Getopt-Long
        perl-Path-Tiny
        perl-Pod-Usage
        perl-Try-Tiny
    # Optional libraries:       # Required by:
        glibmm-devel                # GNOME and KDE layouters, settings.
        libevdev-devel              # libevdev emitter.
        libcap-devel                # Tapper to drop privileges.
        libinput-devel              # libinput listener.
        libudev-devel               # libinput listener.
        libX11-devel                # X Recors listener and Xkb layouter.
        libXtst-devel               # X Test emitter.
    # Optional build tools:     # Required by:
        m4                          # Man page.
        pandoc                      # Man page, metainfo file.
        #~ rpm-build                   # RPM package.                   # N/A
    # Optional Perl modules:    # Required by:
        perl-HTML-Parser            # Metainfo file.
        perl-JSON                   # Metainfo file.
    # Optional test tools:      # Required by:
        #~ appstream                   # RPM package, metainfo file.    # Mised in Void.
        desktop-file-utils          # RPM package, desktop file.
)

# Packages for development:
dev_packages=(
    # Optional build tools:     # Required by:
        #~ copr-cli                    # Release.                       # N/A
        doxygen                     # Source documentation.
        m4                          # HTML pages.
        pandoc                      # HTML pages.
        #~ rsync                       # Release.                       # N/A
        wget                        # HTML pages.
    # Optional test tools:      # Required by:
        aspell                      # Man page, HTML pages.
        aspell-en                   # Man page, HTML pages.
        cppcheck                    # Source.
        man-db                      # Man page.
        #~ rpmlint                     # RPM package.                   # N/A
        tidy                        # HTML pages.
)

# end of file #
