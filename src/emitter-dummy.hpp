/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/emitter-dummy.hpp

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
    `emitter::dummy_t` class interface.

    @sa emitter-dummy.cpp
**/

#ifndef _TAPPER_EMITTER_DUMMY_HPP_
#define _TAPPER_EMITTER_DUMMY_HPP_

#include "base.hpp"
#include "emitter.hpp"

namespace tapper {
namespace emitter {

// -------------------------------------------------------------------------------------------------
// dummy_t
// -------------------------------------------------------------------------------------------------

/**
    Dummy emitter. It does not emit any key events, i. e. it does nothing. All dummy emitter
    methods (except `type()`) exit immediately (some methods may print debug messages, though).
    Tapper uses dummy emitter when no emitter is actually required.
**/
class dummy_t: public object_t, public emitter_t {

    public:

        explicit         dummy_t();
        virtual string_t type()                               override;
        virtual void     start( keys_t const & keys )         override;
        virtual void     emit( events_t const & events )      override;
        virtual void     stop()                               override;

}; // class dummy_t

}; // namespace emitter
}; // namespace tapper

#endif // _TAPPER_EMITTER_DUMMY_HPP_

// end of file //
