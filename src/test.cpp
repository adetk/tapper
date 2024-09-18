/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/test.cpp

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
    Simple assert-based infrastructure for unit tests.
**/

#include "test.hpp"

#include "string.hpp"

namespace tapper {
/** Test guts placed here. **/
namespace test {

#if ENABLE_DEBUG

    /**
        Constructor just executes the given function. Any exceptions thrown from the function, are
        caugth and re-thrown as `std::logic_error`.
    **/
    test_t::test_t(
        std::function< void() > test,   ///< The function to execute.
        char const *            file,
            /**<
                The name of file the constructor is called from, it is expected the caller will
                pass `__FILE__`.
            **/
        int                     line
            /**<
                The line number of the file the constructor is called from, it is expected the
                caller will pass `__LINE__`.
            **/
    ) {
        using tapper::_guts::dprint;
        try {
            dprint( nullptr, file, line, tapper::priority_t::debug, "Executing test…" );
            test();
        } catch ( std::exception const & ex ) {
            dprint(
                nullptr,
                file, line,
                priority_t::error,
                ex.what()
            );
            exit( 255 );
        };
    };

    template<>
    string_t
    str< char const * >(
        char const * const & value
    ) {
        return tapper::q( value );
    };

    template<>
    string_t
    str< string_t >(
        string_t const & value
    ) {
        return tapper::q( value );
    };

    template<>
    string_t
    str< strings_t >(
        strings_t const & value
    ) {
        strings_t strings;
        std::transform(
            value.begin(),
            value.end(),
            std::back_inserter( strings ),
            tapper::q
        );
        return strings.empty() ? "[]" : ( "[ " + join( ", ", strings ) + " ]" );
    };

#endif // ENABLE_DEBUG

}; // namespace test
}; // namespace tapper

// end of file //
