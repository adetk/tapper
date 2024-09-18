#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/setup-altlinux.sh
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
        gcc-c++
        make
        perl-base
        pkg-config
    # Mandatory Perl modules:
        perl-Path-Tiny
        perl-Pod-Usage
        perl-Try-Tiny
    # Optional libraries:       # Required by:
        libglibmm-devel             # GNOME and KDE layouters, settings.
        libcap-devel                # Tapper to drop privileges.
        libevdev-devel              # libevdev emitter.
        libinput-devel              # libinput listener.
        libudev-devel               # libinput listener.
        libX11-devel                # X Record listener, Xkb layouter.
        libXtst-devel               # X Test emitter.
    # Optional build tools:
        m4                          # Man page.
        pandoc                      # Man page, metainfo file.
        #~ rpm-build                   # RPM package.
            # Alt Workstation 10.0 provides `rpmbuild` 4.0.4. Is too old (from 2002), it does not
            # recognize -D option. It seems they build packages with `rpm` command, so there is no
            # reason to install `rpm-build`.
    # Optional Perl modules:    # Required by:
        perl-JSON                   # metainfo file.
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
        rpmlint                     # RPM package.
        tidy                        # HTML pages.
)

# end of file #
