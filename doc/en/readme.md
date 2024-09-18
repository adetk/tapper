<!--
    ---------------------------------------------------------------------- copyright and license ---
    File: doc/en/readme.md
    Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
    This file is part of Tapper.
    SPDX-License-Identifier: GPL-3.0-or-later
    ---------------------------------------------------------------------- copyright and license ---
-->

Why?
----

There are many keyboard layout switchers. All the desktop environments,
including GNOME and KDE, provides either fixed or configurable way to select
keyboard language or layout. For example, in GNOME 3 `Win+Space` *toggles*
keyboard language. If there are more than two languages, `Win+Space` selects
the next language in round-robin manner.

Unlikely to other keyboard layout switchers, Tapper **never toggles** layouts.
Instead, it *dedicates* a key for each layout. A tap on the dedicated key
*always* selects *the same* layout. To select another layout, you have to tap
another key, dedicated for that layout.

Tapper *unique* feature is using modifier keys (such as Shifts, Ctrls, or Alts)
for activating layouts without losing modifier's primary functionality. For
example, `Left Ctrl` tap selects the first layout while `Left Ctrl` still works
as modifier: pressing the `Left Ctrl` key in combination with any other key is
*not* considered as a tap (and does not change keyboard layout) but is
interpreted as usual.


Still why??
-----------

I **want** `Left Ctrl` tap to select English (Latin) keyboard and `Right
Ctrl` tap to select Russian (Cyrillic) keyboard. Xkb is quite flexible, but I
failed to configure Xkb in such a way. It seems it is not possible.

<!-- end of file -->
