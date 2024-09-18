/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/layouter-dummy.hpp

    Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.

    This file is part of Tapper.

    Tapper is free software: you can redistribute it and/or modify it under the terms of the GNU
    General Public License as published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    Tapper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License along with Tapper.  If not,
    see <https://www.gnu.org/licenses/>.

    SPDX-License-Identifier: GPL-3.0-or-later

    ---------------------------------------------------------------------- copyright and license ---
*/

/** @file
    `layouter::dummy_t` class interface.

    @sa layouter-dummy.cpp
**/

#ifndef _TAPPER_LAYOUTER_DUMMY_HPP_
#define _TAPPER_LAYOUTER_DUMMY_HPP_

#include "base.hpp"

#include "layouter.hpp"

namespace tapper {
namespace layouter {

// -------------------------------------------------------------------------------------------------
// dummy_t
// -------------------------------------------------------------------------------------------------

/**
    Dummy layouter.

    Dummy layouter reports no layouts, and does not try to activate layouts. All its methods
    (except `type()`) are stubs. Some methods may print debug messages, thought. Tapper uses dummy
    layouter when no layouter is actually required.
**/
class dummy_t: public object_t, public layouter_t {

    public:

        explicit         dummy_t();
        virtual string_t type()                      override;
        virtual void     activate( layout_t layout ) override;

}; // class dummy_t

}; // namespace layouter
}; // namespace tapper

#endif // _TAPPER_LAYOUTER_DUMMY_HPP_

// end of file //
