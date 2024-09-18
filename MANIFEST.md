<!--
    ---------------------------------------------------------------------- copyright and license ---

    File: MANIFEST.md

    Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.

    This file is part of Tapper.

    Copying and distribution of this file, with or without modification, are permitted in any
    medium without royalty provided the copyright notice and this notice are preserved. This file
    is offered as-is, without any warranty.

    SPDX-License-Identifier: FSFAP

    ---------------------------------------------------------------------- copyright and license ---
-->

MANIFEST
========

This is the complete list of hand-made Tapper source files, covered by Tapper license (see
[LICENSE.md]).

**Note:** Files in `LICENSES` directory are licenses, they are not Tapper source files.

Build scripts
-------------

    bin/build.sh                                GPL-3.0-or-later
    bin/check-cpp-cppcheck.sh                   GPL-3.0-or-later
    bin/check-desktop.sh                        GPL-3.0-or-later
    bin/check-doc-aspell.sh                     GPL-3.0-or-later
    bin/check-doc-hunspell.sh                   GPL-3.0-or-later
    bin/check-html-tidy.sh                      GPL-3.0-or-later
    bin/check-metainfo-appstream.sh             GPL-3.0-or-later
    bin/check-rpm-rpmlint.sh                    GPL-3.0-or-later
    bin/check-srpm-rpmbuild.sh                  GPL-3.0-or-later
    bin/clean.sh                                GPL-3.0-or-later
    bin/copr-build-args.pl                      GPL-3.0-or-later
    bin/install-exec-hook.sh                    GPL-3.0-or-later
    bin/manifest.pl                             GPL-3.0-or-later
    bin/news-to-releases.pl                     GPL-3.0-or-later
    bin/prologue.sh                             GPL-3.0-or-later
    bin/setup-altlinux.sh                       FSFAP
    bin/setup-arch.sh                           FSFAP
    bin/setup-astra.sh                          FSFAP
    bin/setup-debian.sh                         FSFAP
    bin/setup-fedora.sh                         FSFAP
    bin/setup-mageia.sh                         FSFAP
    bin/setup-openmandriva.sh                   FSFAP
    bin/setup-opensuse.sh                       FSFAP
    bin/setup-ubuntu.sh                         FSFAP
    bin/setup-void.sh                           FSFAP
    bin/setup.sh                                FSFAP
    bin/subst.pl                                GPL-3.0-or-later

Various misc files
------------------

    etc/dox.css.in                              FSFAP
    etc/en_US.dic                               FSFAP
    etc/rpmlint.conf                            FSFAP
    etc/doxygen.conf                            FSFAP
    etc/tapper.spec.in                          FSFAP

HTML pages and related stuff
----------------------------

    html/bottom.html.in                         GPL-3.0-or-later
    html/en.md.in                               GPL-3.0-or-later
    html/index.md                               GPL-3.0-or-later
    html/links.md.in                            FSFAP
    html/map.md.in                              GPL-3.0-or-later
    html/ru.md.in                               GPL-3.0-or-later
    html/scripts.js.in                          GPL-3.0-or-later
    html/search.html.in                         GPL-3.0-or-later
    html/style.css.in                           FSFAP
    html/template.html                          GPL-3.0-or-later
    html/top.html.in                            GPL-3.0-or-later

Licenses
--------

This is an exception: Licence files are included into Tapper, but they are *not* Tapper source
files. Also, `GPL-3.0-or-later.reuse` is not an actual license but a stub to shut up the `reuse`
tool.

    LICENSES/FSFAP.md
    LICENSES/GPL-3.0.md
    LICENSES/GPL-3.0-or-later.reuse

M4 macros
---------

    m4/autoblank.m4                             GPL-3.0-or-later
    m4/rdf.m4                                   GPL-3.0-or-later
    m4/v4.m4                                    GPL-3.0-or-later
    m4/vc.m4                                    GPL-3.0-or-later
    m4/vdb.m4                                   GPL-3.0-or-later
    m4/vm.m4                                    GPL-3.0-or-later
    m4/vs.m4                                    GPL-3.0-or-later

Man page
--------

    man/tapper.en.md.in                         GPL-3.0-or-later
    man/tapper.ru.md.in                         GPL-3.0-or-later

Data files
----------

    data/tapper.desktop                         FSFAP
    data/tapper.gschema.xml                     FSFAP
    data/tapper.metainfo.xml                    FSFAP

Documentation
-------------

    doc/en/authors.md                           FSFAP
    doc/en/bugs.md                              GPL-3.0-or-later
    doc/en/install.md                           GPL-3.0-or-later
    doc/en/license.md                           GPL-3.0-or-later
    doc/en/news.md                              FSFAP
    doc/en/readme.md                            GPL-3.0-or-later
    doc/en/todo.md                              GPL-3.0-or-later
    doc/ru/authors.md                           FSFAP
    doc/ru/bugs.md                              GPL-3.0-or-later
    doc/ru/faq.md                               GPL-3.0-or-later
    doc/ru/install.md                           GPL-3.0-or-later
    doc/ru/license.md                           GPL-3.0-or-later
    doc/ru/news.md                              FSFAP
    doc/ru/readme.md                            GPL-3.0-or-later
    doc/ru/todo.md                              GPL-3.0-or-later

C++ sources
-----------

    src/app.cpp                                 GPL-3.0-or-later
    src/app.hpp                                 GPL-3.0-or-later
    src/base.cpp                                GPL-3.0-or-later
    src/base.hpp                                GPL-3.0-or-later
    src/dbus.cpp                                GPL-3.0-or-later
    src/dbus.hpp                                GPL-3.0-or-later
    src/emitter-dummy.cpp                       GPL-3.0-or-later
    src/emitter-dummy.h                         GPL-3.0-or-later
    src/emitter-dummy.hpp                       GPL-3.0-or-later
    src/emitter-libevdev.cpp                    GPL-3.0-or-later
    src/emitter-libevdev.h                      GPL-3.0-or-later
    src/emitter-libevdev.hpp                    GPL-3.0-or-later
    src/emitter-xtest.cpp                       GPL-3.0-or-later
    src/emitter-xtest.h                         GPL-3.0-or-later
    src/emitter-xtest.hpp                       GPL-3.0-or-later
    src/emitter.cpp                             GPL-3.0-or-later
    src/emitter.hpp                             GPL-3.0-or-later
    src/key.hpp                                 GPL-3.0-or-later
    src/layouter-dummy.cpp                      GPL-3.0-or-later
    src/layouter-dummy.h                        GPL-3.0-or-later
    src/layouter-dummy.hpp                      GPL-3.0-or-later
    src/layouter-gnome.cpp                      GPL-3.0-or-later
    src/layouter-gnome.h                        GPL-3.0-or-later
    src/layouter-gnome.hpp                      GPL-3.0-or-later
    src/layouter-kde.cpp                        GPL-3.0-or-later
    src/layouter-kde.h                          GPL-3.0-or-later
    src/layouter-kde.hpp                        GPL-3.0-or-later
    src/layouter-xkb.cpp                        GPL-3.0-or-later
    src/layouter-xkb.h                          GPL-3.0-or-later
    src/layouter-xkb.hpp                        GPL-3.0-or-later
    src/layouter.cpp                            GPL-3.0-or-later
    src/layouter.hpp                            GPL-3.0-or-later
    src/libevdev.cpp                            GPL-3.0-or-later
    src/libevdev.hpp                            GPL-3.0-or-later
    src/libinput.cpp                            GPL-3.0-or-later
    src/libinput.hpp                            GPL-3.0-or-later
    src/linux.cpp                               GPL-3.0-or-later
    src/linux.hpp                               GPL-3.0-or-later
    src/listener-libinput.cpp                   GPL-3.0-or-later
    src/listener-libinput.h                     GPL-3.0-or-later
    src/listener-libinput.hpp                   GPL-3.0-or-later
    src/listener-xrecord.cpp                    GPL-3.0-or-later
    src/listener-xrecord.h                      GPL-3.0-or-later
    src/listener-xrecord.hpp                    GPL-3.0-or-later
    src/listener.cpp                            GPL-3.0-or-later
    src/listener.hpp                            GPL-3.0-or-later
    src/main.cpp                                GPL-3.0-or-later
    src/posix.cpp                               GPL-3.0-or-later
    src/posix.hpp                               GPL-3.0-or-later
    src/privileges.cpp                          GPL-3.0-or-later
    src/privileges.hpp                          GPL-3.0-or-later
    src/range.hpp                               GPL-3.0-or-later
    src/reverse.hpp                             GPL-3.0-or-later
    src/settings.cpp                            GPL-3.0-or-later
    src/settings.hpp                            GPL-3.0-or-later
    src/string.cpp                              GPL-3.0-or-later
    src/string.hpp                              GPL-3.0-or-later
    src/tapper.cpp                              GPL-3.0-or-later
    src/tapper.hpp                              GPL-3.0-or-later
    src/test.cpp                                GPL-3.0-or-later
    src/test.hpp                                GPL-3.0-or-later
    src/timer.cpp                               GPL-3.0-or-later
    src/timer.hpp                               GPL-3.0-or-later
    src/types.cpp                               GPL-3.0-or-later
    src/types.hpp                               GPL-3.0-or-later
    src/x.cpp                                   GPL-3.0-or-later
    src/x.hpp                                   GPL-3.0-or-later
    src/xdg.cpp                                 GPL-3.0-or-later
    src/xdg.hpp                                 GPL-3.0-or-later

Test scripts
------------

    test/cmdline-actions.test                   GPL-3.0-or-later
    test/cmdline-keys.test                      GPL-3.0-or-later
    test/help.test                              GPL-3.0-or-later
    test/list-keys.test                         GPL-3.0-or-later
    test/list-layouts.test                      GPL-3.0-or-later
    test/termination.test                       GPL-3.0-or-later

Configure and make
------------------

    .autom4te.cfg                               FSFAP
    .copr                                       FSFAP
    .hgignore                                   FSFAP
    Makefile.am                                 GPL-3.0-or-later
    configure.ac                                GPL-3.0-or-later

Source documentation
--------------------

    AUTHORS.md                                  FSFAP
    BUGS.md                                     GPL-3.0-or-later
    INSTALL.md                                  GPL-3.0-or-later
    LICENSE.md                                  GPL-3.0-or-later
    MANIFEST.md                                 FSFAP
    META.txt                                    FSFAP
    NEWS.md                                     FSFAP
    README.md                                   GPL-3.0-or-later

<!-- end of file -->
