#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/setup-mageia.sh
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
        #~ m4                          # Man page.             # No pandoc => no need in this tool.
        #~ pandoc                      # Man page, metainfo.   # No pandoc in Mageia.
        rpm-build                   # RPM package.
    # Optional Perl modules:
        #~ 'perl(HTML::Entities)'      # Metainfo file.        # No pandoc => no need in this tool.
        #~ 'perl(JSON)'                # Metainfo file.        # No pandoc => no need in this tool.
    # Optional test tools:      # Required by:
        #~ appstream                   # RPM, metainfo file.   # No pandoc => no need in this tool.
        desktop-file-utils          # RPM, desktop file.
)

# Packages for development:
dev_packages=(
    # Optional build tools:     # Required by:
        doxygen                 # Source documentation.
        #~ m4                          # HTML pages.           # No pandoc => no need in this tool.
        #~ pandoc                      # HTML pages.           # No pandoc in Mageia.
        #~ wget                        # HTML pages.           # No pandoc => no need in this tool.
    # Optional test tools:      # Required by:
        #~ aspell                      # Man page, HTML pages. # No pandoc => no need in this tool.
        #~ aspell-en                   # Man page, HTML pages. # No pandoc => no need in this tool.
        cppcheck                    # Source.
        #~ man-db                      # Man page.             # No pandoc => no need in this tool.
        rpmlint                     # RPM package.
        #~ tidy                        # HTML pages.           # No pandoc => no need in this tool.
)

# end of file #
