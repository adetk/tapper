#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: Makefile.am
#
#   Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
#
#   This file is part of Tapper.
#
#   Tapper is free software: you can redistribute it and/or modify it under the terms of the GNU
#   General Public License as published by the Free Software Foundation, either version 3 of the
#   License, or (at your option) any later version.
#
#   Tapper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
#   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along with Tapper.  If not,
#   see <https://www.gnu.org/licenses/>.
#
#   SPDX-License-Identifier: GPL-3.0-or-later
#
#   ---------------------------------------------------------------------- copyright and license ---

#   TODO:
#       Better html checker.

# Shorter names for frequently used entities:
Name := @PACKAGE_NAME@
name := @PACKAGE_TARNAME@
id   := @PACKAGE_ID@
ver  := @PACKAGE_VERSION@
rel  := @PACKAGE_RELEASE@
if enable_rpm
    dist    := @RPM_DIST@
    disttag := @RPM_DISTTAG@
    arch    := @RPM_ARCH@
endif

# Some useful variables:
hash  := \#
null  :=
space := $(null) $(null)
# Pipe is a problem: If the last command of a pipe returns zero status, make thinks everything is
# ok, while actually some commands may fail. pipefail helps, but it is a bash-specific option.
pipefail := if test -n "$$BASH_VERSION"; then set -o pipefail; fi
prologue = @printf "%s\\n" "[$(MAKELEVEL)] ----- $@ ----- "
define rename =
	$(prologue)
	cp $< $@
endef
define test_prologue =
	$(prologue)
	mkdir -p $(dir $@)
	echo "$(hash)!$(SHELL)" > $@
	chmod a+x $@
endef

# List of phony targets:
PHONY =
    # Automake complains if a target (including special target .PHONY) appears multiple times.
    # Let us collect phony target names in a variable.
# List of source directories (relative to @srcdir@):
SRCDIRS = \
    .           \
    bin         \
    data        \
    etc         \
    html        \
    LICENSES    \
    m4          \
    man         \
    src         \
    test        \
    $(null)
VPATH = $(subst $(space),:,$(strip $(addprefix @srcdir@/, $(SRCDIRS))))
# List of external files (downloaded from the net):
EXTFILES  =
# List of external directories (containing files downloaded from the net):
EXTDIRS   = ext
# List of intermediate files (generated files which are not going to output):
INTFILES  =
# List of intermediate (temporary) directories:
INTDIRS   =
# List of output files:
OUTFILES  =
# List of output directories:
OUTDIRS   =

# GNU make special targets (actually, directives):
.PHONY           : $(PHONY)
.SECONDARY       :
    # GSETTINGS_RULES invalidates .SECONDARY. :-(
.PRECIOUS        : test-suite.log
.DELETE_ON_ERROR :

# Few more directories to install files to:
appdir      = $(datadir)/applications
metainfodir = $(datadir)/metainfo
unitdir     = $(exec_prefix)/lib/systemd/system
userunitdir = $(exec_prefix)/lib/systemd/user
rumandir    = $(mandir)/ru
ruman1dir   = $(rumandir)/man1

# Environment:

export PROLOGUE = set -e; $(pipefail); export SRCDIR='$(srcdir)'; . $(srcdir)/bin/prologue.sh;
# Convert makefile conditionals to environment variables to let tests respect configuration and
# avoid false failures.
if AUTHOR_TESTING
    export AUTHOR_TESTING=1
else
    unexport AUTHOR_TESTING
endif # AUTHOR_TESTING
if enable_emitters
    export ENABLE_EMITTERS=1
else
    unexport ENABLE_EMITTERS
endif # enable_emitters
if enable_gnome
    export ENABLE_GNOME=1
else
    unexport ENABLE_GNOME
endif # enable_gnome
if enable_kde
    export ENABLE_KDE=1
else
    unexport ENABLE_KDE
endif # enable_kde
if with_libcap
    export WITH_LIBCAP=1
else
    unexport WITH_LIBCAP
endif # with_libcap
if with_libevdev
    export WITH_LIBEVDEV=1
else
    unexport WITH_LIBEVDEV
endif # with_libevdev
if with_libinput
    export WITH_LIBINPUT=1
else
    unexport WITH_LIBINPUT
endif # with_libinput
if with_x
    export WITH_X=1
else
    unexport WITH_X
endif # with_x

# Gnome in Fedora 20+ sets bunch of LC_ environment variables. It breaks many tests.
# Let us unset them.
unexport              \
    LC_ALL            \
    LC_ADDRESS        \
    LC_COLLATE        \
    LC_CTYPE          \
    LC_IDENTIFICATION \
    LC_MESSAGES       \
    LC_MEASUREMENT    \
    LC_MONETARY       \
    LC_NAME           \
    LC_NUMERIC        \
    LC_PAPER          \
    LC_TELEPHONE      \
    LC_TIME           \
    $(null)

#
#   Help infrastructure.
#

HELP =
PHONY += help
help :
	$(prologue)
	@echo "Project-specific convenience targets:"
	@echo
	@printf '    %-12s -- %s\n' $(HELP)
	@echo
	@echo "GNU Automake targets (e. g. all, check, dist, distcheck, etc) also work."
	@echo

#
#   All and none
#

# `make all` does not build rpms and html. Let us add target which really builds all, but does not
# run tests.
HELP  += All  "make everything (incl. optional targets) but do not run tests"
HELP  += none "make nothing"
PHONY += All none
All    = all
All  : $(All)
none : $(null)

#
#   Test infrastructure
#

TESTS =
TEST_EXTENSIONS = .test

#
#   Cleaning stuff
#

#   GNU Coding Standards requires 4 clean targets: mostlyclean, clean, distclean, maintainer-clean.

# mostlyclean is a bit weaker version of clean: it may leave some files wich require lot of time to
# rebuilt (and likely not need to be rebuilt). However, I don't have such files, so my mostlyclean
# is (mostly?) identical to clean.
# BTW, automake's mostlyclean deletes test log and result files.
HELP  += mc "= mostlyclean, delete all intermediate files and dirs"
PHONY += mc
mc : mostlyclean
MOSTLYCLEANFILES = $(INTFILES)
MOSTLYCLEANDIRS  = $(INTDIRS)
# Automake takes care about deleting files listed in MOSTLYCLEANFILES.
# But MOSTLYCLEANDIRS is my extension, so I have to handle it:
PHONY += mostlyclean-local
mostlyclean-local :
	$(prologue)
	$(srcdir)/bin/clean.sh -r $(MOSTLYCLEANDIRS)

# clean deletes all the generated files but not results of ./configure (Makefile, config.*,
# configure.*). After clean, make should work (running `configure` is not required).
HELP  += cc "= clean, mostlyclean + delete all output files and dirs"
PHONY += cc
cc : clean
CLEANFILES = $(OUTFILES)
CLEANDIRS  = $(OUTDIRS)
# Again, CLEANDIRS is my extension:
PHONY += clean-local
clean-local :
	$(prologue)
	$(srcdir)/bin/clean.sh -r $(CLEANDIRS)

HELP  += ec "delete all external files and dirs"
PHONY += ec
ec :
	$(prologue)
	$(srcdir)/bin/clean.sh -r $(EXTFILES) $(EXTDIRS)

# distclean should delete results of ./configure, but do not touch ./configure itself.
# After distclean directory should be the same as after extracting distribution tarball (and so,
# user have to run ./configure again).
HELP  += dc "= distclean, clean + ec + delete files made by configure"
PHONY += dc
dc : distclean
DISTCLEANFILES =
DISTCLEANDIRS  = autom4te.cache
    # `autom4te.cache` directory is not built by configure, but it is not a part of distribution.
    # Also, `autom4te` documentation says it can be safely deleted at any time, so let me delete it.
PHONY += distclean-local
distclean-local : ec
	$(prologue)
	$(srcdir)/bin/clean.sh -r $(DISTCLEANDIRS)
	@: # In case of out-of-source build, make leaves `$(builddir)/src` directory. It looks it could
	@: # be deleted, but at this time it is not yet empty, it contains `.deps` directory.

# Both GNU Coding Standards and GNU Automake are not very clear in defining maintainer-clean
# target. It should delete everything which can be reconstructed (probably, with special tools),
# but should not delete configure (which can be reconstructed with autoreconf). Anyway, I do not
# use it. Instead I use veryclean target implemented by `build.sh` which really deletes everything
# generated.
MAINTAINERCLEANFILES =
MAINTAINERCLEANDIRS  =
PHONY += maintainer-clean-local
maintainer-clean-local :
	$(prologue)
	$(srcdir)/bin/clean.sh -r $(MAINTAINERCLEANDIRS)

#
#   Distribution files
#

bin_PROGRAMS = tapper
libraries = $(null)

# Listeners:
if with_libinput
    libraries += listener-libinput.la
endif # with_libinput
if with_x
    libraries += listener-xrecord.la
endif # with_x

# Layouters:
if enable_layouters
libraries += layouter-dummy.la
if enable_gnome
    libraries += layouter-gnome.la
endif # enable_gnome
if enable_kde
    libraries += layouter-kde.la
endif # enable_gnome
if with_x
    libraries += layouter-xkb.la
endif # with_x
endif # enable_layouters

# Emitters:
if enable_emitters
libraries += emitter-dummy.la
if with_libevdev
    libraries += emitter-libevdev.la
endif # with_libevdev
if with_x
    libraries += emitter-xtest.la
endif # with_x
endif # enable_emitters

# Support libraries:
libraries += liblinux.la
if with_x
    libraries += libx.la
endif # with_x

if enable_shared
    pkglib_LTLIBRARIES = $(libraries)
endif # enable_shared
if enable_static
    noinst_LTLIBRARIES = $(libraries)
endif # enable_static

app_DATA      = $(id).desktop
if enable_metainfo
    metainfo_DATA = $(id).metainfo.xml
endif # enable_metainfo
if with_glib
    gsettings_SCHEMAS = $(id).gschema.xml
endif # with_glib
if enable_man
    man1_MANS  = \
        tapper.en.man                           \
        tapper.ru.man                           \
        $(null)
endif # enable_man
doc_DATA = \
    AUTHORS.md                                  \
    LICENSE.md                                  \
    LICENSES/FSFAP.md                           \
    LICENSES/GPL-3.0.md                         \
    NEWS.md                                     \
    $(null)

# Files to include into source package:
manifest := $(shell $(PERL) $(srcdir)/bin/manifest.pl --no-check $(srcdir)/MANIFEST.md)
EXTRA_DIST = $(manifest)

HELP  += manifest "make plain manifest.lst file"
PHONY += manifest
manifest : manifest.lst
manifest.lst :
	$(prologue)
	printf "%s\n" $(manifest) > $@

if AUTHOR_TESTING
    INTFILES += manifest.test
    TESTS    += manifest.test
    manifest.test : MANIFEST.md
	    $(test_prologue)
	    echo "$(PERL) $(srcdir)/bin/manifest.pl --check --no-list $<" >> $@
endif # AUTHOR_TESTING

OUTFILES += $(PROGRAMS) $(SCRIPTS) $(DATA) $(gsettings_SCHEMAS) $(MANS)
# build.sh sets DESTDIR variable to "$builddir/install". But it is too dangerous to declare
# $(DESTDIR) as output directory. In another environment (e. g. manual run without build.sh)
# DESTDIR can be "/" or "/usr/local". So let us use "install". It will work for build.sh, and
# harmless in other curcumstances.
OUTDIRS  += install
    # TODO: Isn't it an intermediate dir?

#
#   Install
#

install-exec-hook :
	$(prologue)
	SETCAP=$(SETCAP) CHGRP=$(CHGRP) CHMOD=$(CHMOD) \
	    $(srcdir)/bin/install-exec-hook.sh $(DESTDIR)$(bindir)/$(name)

if enable_man
# Do not indent content of `if enadle_man`! Automake fails to recongnize indented hook!
# Looks like Automake does not support installing non-English man pages out of the box.
# Let's implement rearrange man pages properly.
# Automake can install either plain or gzipped man pages. `install-data-hook` should properly
# handle both variants.
install-data-hook :
	$(prologue)
	mkdir -p -m u=rwx,go=rx $(DESTDIR)$(ruman1dir)
	for sfx in '' '.gz'; do                                         \
	    [[ ! -f $(DESTDIR)$(man1dir)/$(name).en.1$$sfx ]] ||        \
	        mv  $(DESTDIR)$(man1dir)/$(name).en.1$$sfx              \
	            $(DESTDIR)$(man1dir)/$(name).1$$sfx;                \
	    [[ ! -f $(DESTDIR)$(man1dir)/$(name).ru.1$$sfx ]] ||        \
	        mv  $(DESTDIR)$(man1dir)/$(name).ru.1$$sfx              \
	            $(DESTDIR)$(ruman1dir)/$(name).1$$sfx;              \
	done
endif # enable_man

#
#   Tests
#

# Let me add two aliases for `distcheck` target,
# because I often type `check-dist` instead of `distcheck`.
HELP  += dist-check "= distcheck"
PHONY += dist-check check-dist
check-dist : distcheck
dist-check : distcheck

#
#   Program and libraries
#

ACLOCAL_AMFLAGS = -I _m4
AM_CPPFLAGS = \
    $(LIBCAP_CFLAGS)                    \
    $(GLIBMM_CFLAGS) $(GIOMM_CFLAGS)    \
    $(LIBEVDEV_CFLAGS)                  \
    $(UDEV_CFLAGS) $(LIBINPUT_CFLAGS)   \
    $(X11_CFLAGS) $(XTST_CFLAGS)        \
    -DDATADIR='"$(datadir)"'            \
    $(null)
AM_CXXFLAGS = -std=c++11 -Wall $(PTHREAD_CFLAGS)
AM_LDFLAGS  = $(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

tapper_SOURCES = \
    src/app.cpp                         \
    src/base.cpp                        \
    src/emitter.cpp                     \
    src/layouter.cpp                    \
    src/listener.cpp                    \
    src/main.cpp                        \
    src/posix.cpp                       \
    src/privileges.cpp                  \
    src/settings.cpp                    \
    src/string.cpp                      \
    src/tapper.cpp                      \
    src/test.cpp                        \
    src/timer.cpp                       \
    src/types.cpp                       \
    src/xdg.cpp                         \
    $(null)
if with_glib
    tapper_SOURCES += src/dbus.cpp
endif # with_glib
tapper_LDADD = $(LIBCAP_LIBS) $(GLIBMM_LIBS) $(GIOMM_LIBS)

# Listeners:
if with_libinput
    BUILT_SOURCES                    = input-event-names.def
    # Libinput listener:
    listener_libinput_la_SOURCES     = src/listener-libinput.cpp src/libinput.cpp
    listener_libinput_la_LDFLAGS     = -module -avoid-version
    listener_libinput_la_LIBADD      = $(UDEV_LIBS) $(LIBINPUT_LIBS) liblinux.la
    tapper_LDADD                    += listener-libinput.la
endif # with_libinput
if with_x
    # XRecord listener:
    listener_xrecord_la_SOURCES      = src/listener-xrecord.cpp
    listener_xrecord_la_LDFLAGS      = -module -avoid-version
    listener_xrecord_la_LIBADD       = libx.la
    tapper_LDADD                    += listener-xrecord.la
endif # with_x

# Layouters:
if enable_layouters
# Dummy layouter:
layouter_dummy_la_SOURCES            = src/layouter-dummy.cpp
layouter_dummy_la_LDFLAGS            = -module -avoid-version
layouter_dummy_la_LIBADD             =
tapper_LDADD                        += layouter-dummy.la
if enable_gnome
    # GNOME layouter:
    layouter_gnome_la_SOURCES        = src/layouter-gnome.cpp
    layouter_gnome_la_LDFLAGS        = -module -avoid-version
    layouter_gnome_la_LIBADD         = $(GLIBMM_LIBS) $(GIOMM_LIBS)
    tapper_LDADD                    += layouter-gnome.la
endif # enable_gnome
if enable_kde
    # KDE layouter:
    layouter_kde_la_SOURCES          = src/layouter-kde.cpp
    layouter_kde_la_LDFLAGS          = -module -avoid-version
    layouter_kde_la_LIBADD           = $(GLIBMM_LIBS) $(GIOMM_LIBS)
    tapper_LDADD                    += layouter-kde.la
endif # enable_kde
if with_x
    # Xkb layouter:
    layouter_xkb_la_SOURCES          = src/layouter-xkb.cpp
    layouter_xkb_la_LDFLAGS          = -module -avoid-version
    layouter_xkb_la_LIBADD           = libx.la
    tapper_LDADD                    += layouter-xkb.la
endif # with_x
endif # enable_layouters

# Emitters:
if enable_emitters
# Dummy emitter:
emitter_dummy_la_SOURCES             = src/emitter-dummy.cpp
emitter_dummy_la_LDFLAGS             = -module -avoid-version
emitter_dummy_la_LIBADD              =
tapper_LDADD                        += emitter-dummy.la
if with_libevdev
    # libevdev emitter:
    emitter_libevdev_la_SOURCES      = src/emitter-libevdev.cpp src/libevdev.cpp
    emitter_libevdev_la_LDFLAGS      = -module -avoid-version
    emitter_libevdev_la_LIBADD       = $(LIBEVDEV_LIBS)
    tapper_LDADD                    += emitter-libevdev.la
endif # with libevdev
if with_x
    # xtest emitter:
    emitter_xtest_la_SOURCES         = src/emitter-xtest.cpp
    emitter_xtest_la_LDFLAGS         = -module -avoid-version
    emitter_xtest_la_LIBADD          = libx.la
    tapper_LDADD                    += emitter-xtest.la
endif # with x
endif # enable_emitters

# Support libraries:
# Linux support library (it is used by libinput listener and libevdev emitter):
liblinux_la_SOURCES                  = src/linux.cpp
liblinux_la_LDFLAGS                  = -avoid-version
liblinux_la_LIBADD                   =
if with_x
    # X support library (it is used by XRecord listener and Xkb layouter):
    libx_la_SOURCES                  = src/x.cpp
    libx_la_LDFLAGS                  = -avoid-version
    libx_la_LIBADD                   = $(X11_LIBS) $(XTST_LIBS)
endif # with_x

INTFILES += input-event-names.def
if HAVE_LINUX_INPUT_EVENT_CODES_H
    #   Extract key and button names from input-event-codes.h.
    INTFILES += $(addprefix input-event-names.def, .cpp .lst .h)
    input-event-names.def :
		$(prologue)
		@: # Prepare the source:
		echo "#include <linux/input-event-codes.h>" > $@.cpp
		@: # Get list of headers included into the source:
		$(CXX) -H -E $@.cpp > /dev/null 2> $@.lst
		@: # Each file in the list is full path, prefixed with seriaes of dots to visualize the
		@: # nesting level. Since `linux/input-event-codes.h` is the header directly included into
		@: # the source, it is prefixed with *one* dot. Also, since `linux/input-event-codes.h` is
		@: # the only header directly included into the source, only one such file is expected.
		@: # Ignore files prefixed with more than one dot, if any.
		$(PERL) -n \
			-e 'm{^\. (.*)$$} and do { open $$h, "<$$1"; local $$/ = undef; print <$$h>; }' \
			$@.lst > $@.h
		@: # Extract key and button names from the header. Ignore few names which are not names
		@: # of real keys but convenience constants: KEY_RESERVED, KEY_MAX, etc.
		$(PERL) -n \
			-e 'BEGIN { $$ignore = qr{\AKEY_(?:RESERVED|MIN_INTERESTING|MAX|CNT)\z}; }'            \
			-e 'm{^\h*\#define\h+((?:KEY|BTN)_\w*)} && $$1 !~ $$ignore and print( "X( $$1 )\n" );' \
			$@.h > $@
else
    # If `linux/input-event-codes.h` is not available, generate empty dummy file.
    input-event-names.def :
		$(prologue)
		echo > $@
endif

if AUTHOR_TESTING
    cppcheck_tests := $(tapper_SOURCES:=.cppcheck.test)
    INTFILES += $(cppcheck_tests)
    TESTS    += $(cppcheck_tests)
    $(cppcheck_tests) : %.cpp.cppcheck.test : %.cpp
		$(test_prologue)
		echo "CPPCHECK='$(CPPCHECK)' $(srcdir)/bin/check-cpp-cppcheck.sh $<" >> $@
endif AUTHOR_TESTING

TESTS += \
    help.test               \
    cmdline-keys.test       \
    cmdline-actions.test    \
    list-keys.test          \
    list-layouts.test       \
    termination.test        \
    $(null)

#
#   data files
#

data   = $(app_DATA) $(metainfo_DATA) $(gsettings_SCHEMAS)
HELP  += data "make data files"
PHONY += data
All   += data
data : $(data)

#   Data files must use application id in their names. However, using application id in source
#   is not convenient: application id may be long, and is subject to change.
$(id).%.in : $(name).% ; $(rename)

if enable_metainfo

    $(id).metainfo.xml : releases.xml
    releases.xml : NEWS.md
	    $(prologue)
	    $(PANDOC) -f markdown -t json -o news.json $<
	    $(PERL) $(srcdir)/bin/news-to-releases.pl \
	        --limit=10 --url='@PACKAGE_URL@news.html' -o $@ news.json

    appstream_tests := $(id).metainfo.test
    INTFILES += $(appstream_tests)
    TESTS    += $(appstream_tests)
    $(appstream_tests) : %.metainfo.test : %.metainfo.xml
	    $(test_prologue)
	    echo "APPSTREAM='$(APPSTREAM)' $(srcdir)/bin/check-metainfo-appstream.sh $<" >> $@

endif # enable_metainfo

desktop_tests := $(id).desktop.test
INTFILES += $(desktop_tests)
TESTS    += $(desktop_tests)
$(desktop_tests) : %.desktop.test : %.desktop
	$(test_prologue)
	echo "DESKTOP_FILE_VALIDATE='$(DESKTOP_FILE_VALIDATE)' $(srcdir)/bin/check-desktop.sh $<" >> $@

HELP  += data-check "check data files"
PHONY += data-check
data_tests := $(desktop_tests) $(appstream_tests)
data-check : $(data) $(data_tests)
	$(prologue)
	$(MAKE) $(AM_MAKEFLAGS) TESTS='$(data_tests)' check

#
#   doc files
#

#   Names of documentation files accordingly to the GNU Coding Standard must be in upper case:
#   BUGS, NEWS, etc. However, I want html pages names are in lower case. Also, I want these files
#   are subject to substitution.
authors.md.in  : AUTHORS.md  ; $(rename)
bugs.md.in     : BUGS.md     ; $(rename)
fsfap.md       : FSFAP.md    ; $(rename)
gpl-3.0.md     : GPL-3.0.md  ; $(rename)
install.md.in  : INSTALL.md  ; $(rename)
license.md.in  : LICENSE.md  ; $(rename)
manifest.md.in : MANIFEST.md ; $(rename)
news.md.in     : NEWS.md     ; $(rename)

INTFILES += \
    $(foreach base, authors bugs fsfap gpl-3.0 install license manifest news, $(base).md $(base).md.in)

#
#   man pages
#

if enable_man

HELP  += man "make man pages"
PHONY += man
All   += man
man : $(MANS)
INTFILES += $(MANS:.man=.md)
$(MANS) : %.man : %.md bugs.md subst.txt rdf.m4
	$(prologue)
	$(pipefail);                                                                                   \
	cat $<                                                                                         \
	| $(PERL) $(srcdir)/bin/subst.pl -f subst.txt -                                                \
	| $(M4) -P -I $(srcdir) $(filter %/rdf.m4, $^) -                                               \
	| $(PERL) -p                                                                                   \
		-e 'BEGIN { $$/ = undef; };'                                                               \
		-e 's/\[([\w\s]+)\]\[\]/$$1/g;                                                      # 1'   \
		-e 's/^###\h+(.*)$$/**$$1**/mg;                                                     # 2'   \
		-e 's/^((?:NAME\n====|ИМЯ\n===)\n\n\*\*`[a-z ]+`\*\*) — /$$1 - /m;                  # 3'   \
		-e 's/((?:\*\*)?)(`[^`]+`)\1((?:\{\.k\})?)/ "$$1$$3" ? "$$1$$2$$1$$3" : "‘$$2’"/eg; # 4'   \
	| $(PANDOC) --read=markdown-smart --write=man -s                                               \
	| $(PERL) -p                                                                                   \
		-e 'BEGIN { $$/ = undef; };'                                                               \
		-e 's/\\\[u([0-9a-f]{4})\]/chr( oct( "x$$1" ) )/gei;                                # 6'   \
	> $@

#   1.  I generate both man page and HTML pag from the same source. But man page and HTML page
#       differ: Innier-text hyperlinks are ok for HTML page, but construction
#           “How to Report Bugs Effectively (https://www.chiark.greenend.org.uk/...)”
#       in man page looks unly, I want to specify the article URL once in section "See also".
#   2.  In the man page, level 2 and 3 headings are not distinguishable. Let's replace level 3
#       headings with bold font.
#   3.  Replace em-dash with dash in “NAME” section, otherwise `apropos` will not work.
#   4.  In HTML page `code` is rendered using monospace font and different background color. In man
#       page such formatting is not possible. Let's highlight code fragments with single quotes:
#       ‘code’. However, do not add quotes to fragments, which already highlighted with bold font
#       (**`--option`**) and to key names (`Ctrl`{.k}).
#   5.  pandoc 2.14.0.3 escaped all russian letters (writes them as escape sequences `\[uHHHH]`,
#       where HHHH is 4 hex digits). It makes man file 3x larger. Lets' de-escape escape sequences.

endif # enable_man

#
#   Spell check
#

if AUTHOR_TESTING
    doc_files := $(doc_DATA) $(MANS) BUGS.md INSTALL.md README.md
    spell_tests := $(doc_files:=.spell.test)
    INTFILES += $(spell_tests)
    TESTS    += $(spell_tests)
    $(spell_tests) : %.spell.test : % | en.pws
		$(test_prologue)
		echo \
			"ASPELL='$(ASPELL)'"     \
			"MAN='$(MAN)'"           \
			"PANDOC='$(PANDOC)'"     \
			"PERL='$(PERL)'"         \
			"$(srcdir)/bin/check-doc-aspell.sh $<" >> $@

    HELP  += spell-check "check spelling of the docs"
    PHONY += spell-check check-spell spellcheck
    check-spell : spell-check
    spellcheck  : spell-check
    spell-check : $(doc_DATA) $(MANS)
		$(prologue)
		$(MAKE) $(AM_MAKEFLAGS) TESTS='$(spell_tests)' check
    en.pws : en_US.dic
		$(prologue)
		echo "personal_ws-1.1 en 0 UTF-8" > $@.tmp
		grep -v -E '^#|^$$' $< >> $@.tmp
		mv $@.tmp $@
endif # AUTHOR_TESTING

#
#   tgz
#

distdir = $(name)-$(ver)-$(rel)
tgz := $(distdir).tar.gz
OUTFILES += $(tgz)
HELP  += tgz "make source tarball"
PHONY += tgz
All   += tgz
tgz : $(tgz)
$(tgz) : $(DISTFILES)
	$(prologue)
	$(MAKE) $(AM_MAKEFLAGS) dist-gzip am__post_remove_distdir='@:'
INTDIRS += $(distdir)

HELP += tgz-check "unpack tarball and make vc All check dist-check"
PHONY += check-tgz tgz-check
check-tgz : tgz-check
tgz-check : $(tgz) tgz-check.tmp/.dir
	$(prologue)
	$(srcdir)/bin/clean.sh -x .dir $@.tmp
	$(TAR) xaf $(tgz) -C $@.tmp
	@: # `make distcheck` runs `make install`, which will fail if run by the non-privileged user.
	@: # TODO: Fix it.
	cd $@.tmp/$(distdir) && MAKE='$(MAKE)' ./bin/build.sh vc All check distcheck
INTDIRS += tgz-check.tmp

if UPLOAD

HELP  += tgz-upload "upload tarball"
PHONY += tgz-upload upload-tgz
upload-tgz : tgz-upload
tgz-upload : $(tgz)
	$(prologue)
	$(UPLOAD) $< @PACKAGE_SF_UPLOAD_FILES@

endif # UPLOAD

#
#   rpms
#

if enable_rpm

spec := $(name).spec
srpm := $(spec:.spec=)-$(ver)-$(rel)$(dist).src.rpm
brpm := $(srpm:.src.rpm=$(disttag).$(arch).rpm)
rpms := $(brpm) $(srpm)
OUTFILES += $(rpms)
INTFILES += $(spec)
HELP  += brpm "make binary rpm package"
HELP  += srpm "make source rpm package"
HELP  += rpms "make both binary and source rpm packages"
PHONY += brpm srpm rpms rpm
All   += rpms
brpm : $(brpm)
srpm : $(srpm)
rpms : $(rpms)
rpm  : rpms

# Note: rpmbuild invokes make.
# It causes problem, because this make saves some flags (including a list of tests to run) in
# environment variable MAKEFLAGS. Thus, inner make tries to execute the same tests as outer one,
# but it should not, because in spec file I explicitly disabled html and rpm. Clearing MAKEFLAGS
# variable avoids the problem.
rpmbuild =                                                  \
	MAKEFLAGS=                                              \
	$(RPMBUILD)                                             \
	    -D '_topdir $(abs_builddir)/rpmbuild-$(notdir $@)'  \
	    -D '_specdir $(abs_builddir)'                       \
	    -D '_sourcedir $(abs_builddir)'                     \
	    $(null)
INTDIRS += rpmbuild-$(brpm) rpmbuild-$(srpm)

$(brpm) : $(spec) $(tgz)
	$(prologue)
	$(rpmbuild) -bb $<
	cp rpmbuild-$(notdir $@)/RPMS/$(arch)/$(notdir $@) $@

$(srpm) : $(spec) $(tgz)
	$(prologue)
	$(rpmbuild) -bs $<
	cp rpmbuild-$(notdir $@)/SRPMS/$(notdir $@) $@

if AUTHOR_TESTING
    rpmlint_tests := $(rpms:=.rpmlint.test)
    INTFILES += $(rpmlint_tests)
    TESTS    += $(rpmlint_tests)
    $(rpmlint_tests) : %.rpm.rpmlint.test : %.rpm
		$(test_prologue)
		echo "RPMLINT='$(RPMLINT)' $(srcdir)/bin/check-rpm-rpmlint.sh $<" >> $@
    INTDIRS += check-rpm-rpmlint.sh.tmp
endif # AUTHOR_TESTING

rpmbuild_tests := $(srpm:=.rpmbuild.test)
INTFILES += $(rpmbuild_tests)
TESTS    += $(rpmbuild_tests)
$(rpmbuild_tests) : %.src.rpm.rpmbuild.test : %.src.rpm
	$(test_prologue)
	echo "RPMBUILD='$(RPMBUILD)' $(srcdir)/bin/check-srpm-rpmbuild.sh $<" >> $@
INTDIRS += check-srpm-rpmbuild.sh.tmp

HELP  += rpms-check "check rpm packages"
PHONY += rpms-check rpm-check check-rpms check-rpm
rpm-check  : rpms-check
check-rpms : rpms-check
check-rpm  : rpms-check
rpms-check : $(rpms)
	$(prologue)
	$(MAKE) $(AM_MAKEFLAGS) TESTS='$(rpmlint_tests) $(rpmbuild_tests)' check

if UPLOAD

HELP  += srpm-upload "upload source rpm"
PHONY += srpm-upload srpm-up upload-srpm
srpm-up     : srpm-upload
upload-srpm : srpm-upload
srpm-upload : $(srpm)
	$(prologue)
	$(UPLOAD) $< @PACKAGE_SF_UPLOAD_FILES@

endif # UPLOAD

if COPR

HELP  += copr "build rpms in copr"
PHONY += copr
copr : $(srpm)
	$(prologue)
	$(COPR) build $$( $(srcdir)/bin/copr-build-args.pl "$(srcdir)" ) "@PACKAGE_COPR_PRJ@" $<

endif # COPR

endif # enable_rpm

#
#   html pages
#

if enable_html

css  := style.css
html := \
    authors.html    \
    bugs.html       \
    en.html         \
    fsfap.html      \
    gpl-3.0.html    \
    index.html      \
    install.html    \
    license.html    \
    manifest.html   \
    map.html        \
    news.html       \
    ru.html         \
    tapper.en.html  \
    tapper.ru.html  \
    $(null)
js  := scripts.js
svg := \
    gplv3-or-later.svg      \
    russia.svg              \
    sf.svg                  \
    $(null)
site := $(html) $(css) $(js) $(svg)
OUTFILES += $(site)
HELP  += html "make html pages"
PHONY += html
All   += html
# TODO: html is standard Automake target. Probably I have to change name.
html : $(site)

$(html) : %.html : %.md links.md rdf.m4 top.html bottom.html template.html | $(css)
	$(prologue)
	$(pipefail);                                                                                   \
	cat links.md $<                                                                                \
	| $(M4) -P -I $(srcdir) $(filter %/rdf.m4, $^) -                                               \
	| $(PERL) -p                                                                                   \
	    -e 'BEGIN { $$/ = undef; };'                                                               \
	    -e 's/^((\d+(?:\.\d+)+) @ \d\d\d\d-\d\d-\d\d)\n(-{13,})\n/"$$1 {id=\"$$2\"}\n$$3\n"/gem;'  \
	| $(PANDOC)                                                                                    \
	    $(if $(filter $(name).%.md, $<), --shift-heading-level-by=1)                               \
	    --read=markdown-smart-implicit_figures                                                     \
	    --write=html                                                                               \
	    -V css=$(css)                                                                              \
	    $(if $(filter %/index.md, $<),                                                             \
	        -V header-includes='<meta http-equiv="refresh" content="0;URL=en.html"/>')             \
	    -V header-includes='<script src="scripts.js"></script>'                                    \
	    -B top.html                                                                                \
	    -A bottom.html                                                                             \
	    --section-divs                                                                             \
	    --template=$(filter %/template.html, $^)                                                   \
	    --email-obfuscation=references                                                             \
	> $@

index.html  : map.md search.html
map.html    : search.html
tapper.html : bugs.md

$(svg) : % : ext/% ; $(rename)

ext/sf.svg :  ext/.dir
	$(prologue)
	$(WGET) -O $@ 'https://sourceforge.net/sflogo.php?type=17&group_id=0'
    # Downloaded file may be older than etx/.dir file. In such a case make will download it again
    # and again. Touch it to avoid reloading.
	touch $@

ext/russia.svg :  ext/.dir
	$(prologue)
	$(WGET) -O $@ 'https://upload.wikimedia.org/wikipedia/commons/f/f3/Flag_of_Russia.svg'
	touch $@

ext/gplv3-or-later.svg :  ext/.dir
	$(prologue)
	$(WGET) -O $@ 'https://www.gnu.org/graphics/gplv3-or-later.svg'
	touch $@

$(srcdir)/README.txt : en.md links.md subst.txt rdf.m4
	$(prologue)
	$(pipefail);                                                                                   \
	cat $< links.md                                                                                \
	|                                                                                              \
	$(PERL) $(srcdir)/bin/subst.pl -f subst.txt                                                    \
	|                                                                                              \
	$(M4) -P -D RDF_ENABLE=0 -D README=1 $(filter %/rdf.m4, $^) -  \
	|                                       \
	$(PANDOC)                               \
	    -V css=$(css)                       \
	    --read=markdown                     \
	    --write=markdown-footnotes          \
	    --strip-comments                    \
	    --reference-links                   \
	    --columns=80                        \
	|                                       \
	$(PERL) -p                                                                              \
	    -e 's{^(\[1]\ )}{Footnotes\n---------\n\n$$1};'                                     \
	    -e 's{^  \[[^\[\]]+\]: (?!https?://)[^ ]+\n}{};'                                    \
	    -e 'not $$links and s{^(  \[[^\[\]]+\]: )}{Links\n-----\n\n$$1} and $$links = 1;'   \
	> $@

if AUTHOR_TESTING
    html_tidy_tests := $(html:=.tidy.test)
    INTFILES += $(html_tidy_tests)
    TESTS    += $(html_tidy_tests)
    $(html_tidy_tests) : %.html.tidy.test : %.html
		$(test_prologue)
		echo "TIDY='$(TIDY)' $(srcdir)/bin/check-html-tidy.sh $<" >> $@
    INTDIRS += check-html-tidy.sh.tmp

    HELP  += html-check "check html pages"
    PHONY += html-check check-html
    check-html : html-check
    html-check : $(html)
		$(prologue)
		$(MAKE) $(AM_MAKEFLAGS) TESTS='$(html_tidy_tests)' check
endif # AUTHOR_TESTING

if UPLOAD

HELP  += html-upload "upload all html pages to the web site"
PHONY += html-upload upload-html
upload-html : html-upload

if RSYNC
# It is quite tricky solution. I want rsync to delete any extra files on target. This is possible
# only when synching entire directory, not list of files. But our build directory contains too many
# files which should not be copied to web site. One straightforward approach is creating special
# directory and populating it with only required files. Another approach is sync the build
# directory but explicitly specify included files and exclude all.
html-upload : site.lst $(site)
	$(prologue)
	$(UPLOAD) --include-from=$< '--exclude=*' --delete-excluded . @PACKAGE_SF_UPLOAD_HTML@
endif # RSYNC

if SCP
html-upload : $(site)
	$(prologue)
	$(UPLOAD) $^ @PACKAGE_SF_UPLOAD_HTML@
endif # SCP

INTFILES += site.lst
site.lst :
	$(prologue)
	$(pipefail); \
	echo "$(site)" | sed -e 's| |\n|g' > $@

endif # UPLOAD

endif # enable_html

#
#   Source documentation.
#

if DOXYGEN

doxyfile.in : doxygen.conf ; $(rename)

HELP  += dox "build source code documentation"
PHONY += dox
dox : doxyfile dox.css
	$(prologue)
	$(DOXYGEN) $<

endif # DOXYGEN

#
#   General rules
#

INTFILES += $(filter-out $(OUTFILES), $(patsubst %.in, %, $(filter %.in, $(notdir $(manifest)))))
% : %.in subst.txt
	$(prologue)
	$(PERL) $(srcdir)/bin/subst.pl -f subst.txt -o $@ $<

INTFILES += subst.txt
subst.txt :
	$(prologue)
	{ \
		echo 'name=$(name)';                                            \
		echo 'Name=$(Name)';                                            \
		echo 'ver=$(ver)';                                              \
		echo 'rel=$(rel)';                                              \
		echo 'PACKAGE_BRPM=$(brpm)';                                    \
		echo 'PACKAGE_BUGREPORT=@PACKAGE_BUGREPORT@';                   \
		echo 'PACKAGE_COPR_PKG=@PACKAGE_COPR_PKG@';                     \
		echo 'PACKAGE_COPR_PRJ=@PACKAGE_COPR_PRJ@';                     \
		echo 'PACKAGE_COPR_REPO=@PACKAGE_COPR_REPO@';                   \
		echo 'PACKAGE_COPYRIGHT_HOLDER=@PACKAGE_COPYRIGHT_HOLDER@';     \
		echo 'PACKAGE_COPYRIGHT_YEAR=@PACKAGE_COPYRIGHT_YEAR@';         \
		echo 'PACKAGE_DATE=@PACKAGE_DATE@';               \
		echo 'PACKAGE_EMAIL=@PACKAGE_EMAIL@';             \
		echo 'PACKAGE_GSCHEMA_ID=$(id)';                  \
		echo 'PACKAGE_GSCHEMA_PATH=/$(subst .,/,$(id))/'; \
		echo 'PACKAGE_ID=$(id)';                          \
		echo 'PACKAGE_LICENSE=@PACKAGE_LICENSE@';         \
		echo 'PACKAGE_LICENSE_URL=@PACKAGE_LICENSE_URL@'; \
		echo 'PACKAGE_NAME=@PACKAGE_NAME@';               \
		echo 'PACKAGE_NVR=$(name)-$(ver)-$(rel)';         \
		echo 'PACKAGE_RELEASE=@PACKAGE_RELEASE@';         \
		echo 'PACKAGE_SUMMARY=@PACKAGE_SUMMARY@';         \
		echo 'PACKAGE_SF_FILES=@PACKAGE_SF_FILES@';       \
		echo 'PACKAGE_SF_FORUMS=@PACKAGE_SF_FORUMS@';     \
		echo 'PACKAGE_SF_REVIEWS=@PACKAGE_SF_REVIEWS@';   \
		echo 'PACKAGE_SF_SUMMARY=@PACKAGE_SF_SUMMARY@';   \
		echo 'PACKAGE_SF_TICKETS=@PACKAGE_SF_TICKETS@';   \
		echo 'PACKAGE_SRPM=$(srpm)';                      \
		echo 'PACKAGE_STRING=@PACKAGE_STRING@';           \
		echo 'PACKAGE_TARNAME=@PACKAGE_TARNAME@';         \
		echo 'PACKAGE_TARBALL=$(tgz)';                    \
		echo 'PACKAGE_TRIAL=@PACKAGE_TRIAL@';             \
		echo 'PACKAGE_URL=@PACKAGE_URL@';                 \
		echo 'PACKAGE_VERSION=@PACKAGE_VERSION@';         \
		echo 'RELEASES=$$(<releases.xml)';                \
		echo 'bindir=$(bindir)';                          \
		echo 'datadir=$(datadir)';                        \
		echo 'sbindir=$(sbindir)';                        \
		echo 'srcdir=$(srcdir)';                          \
		echo 'sysconfdir=$(sysconfdir)';                  \
	} > $@

%.gz : %
	$(prologue)
	gzip $<

%/.dir :
	$(prologue)
	test -d $(dir $@) || mkdir $(dir $@)
	echo > $@

if with_glib
@GSETTINGS_RULES@
endif # with_glib

libtool: $(LIBTOOL_DEPS)
	$(prologue)
	$(SHELL) ./config.status libtool

# end of file #
