/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/emitter-xtest.hpp

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
    `emitter::xtest_t` class interface.

    @sa emitter-xtest.cpp
**/

#ifndef _TAPPER_EMITTER_XTEST_HPP_
#define _TAPPER_EMITTER_XTEST_HPP_

#include "base.hpp"
#include "emitter.hpp"

#include "x.hpp"

namespace tapper {
namespace emitter {

// -------------------------------------------------------------------------------------------------
// xtest_t
// -------------------------------------------------------------------------------------------------

/** XTest emitter. **/
class xtest_t: public object_t, public emitter_t {

    public:

        explicit         xtest_t();
        virtual string_t type()                               override;
        virtual void     start( keys_t const & keys )         override;
        virtual void     emit( events_t const & event )       override;
        virtual void     stop()                               override;

    private:

        x::display_t    _display;
        x::test_t       _test;

}; // class xtest_t

}; // namespace emitter
}; // namespace tapper

#endif // _TAPPER_EMITTER_XTEST_HPP_

// end of file //
