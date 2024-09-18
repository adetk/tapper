/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/test.hpp

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

    Usage:

    @code
        bool odd( int x ) { return x % 2; };
        int  dbl( int x ) { return x + x; };
        void bug() { throw std::logic_error( "oops" ); };

        TEST(
            ASSERT( odd( 1 ) );
            ASSERT( not odd( 2 ) );
            ASSERT_EQ( dbl( 0 ), 0 );
            ASSERT_EQ( dbl( 1 ), 2 );
            ASSERT_EX( bug(), std::logic_error, "oops" );
        );
    @endcode

    Macro `TEST()` produces no code if `ENABLE_DEBUG` macro is not defined or zero.

    @sa test.cpp
**/

#ifndef _TAPPER_TEST_HPP_
#define _TAPPER_TEST_HPP_

#include "config.h"

#if ENABLE_DEBUG

    #include "base.hpp"

    #include <functional>   // std::function
    #include <regex>
    #include <stdexcept>    // std::logic_error

    namespace tapper {
    namespace test {

        /*
            This macro is excluded from doxygen documentation because Doxygen treats every `TEST()`
            call as a declaration of `TEST()` function and floods documentation with lots of
            `TEST()` functions.
        */
        #define TEST( ... )            \
            TEST_( __COUNTER__, ## __VA_ARGS__ )

        /** `TEST()` guts, do not use it directly. **/
        #define TEST_( COUNTER, ... )  \
            TEST__( COUNTER, ## __VA_ARGS__ )

        /** `TEST_()` guts, do not use it directly. **/
        #define TEST__( COUNTER, ... ) \
            static tapper::test::test_t t##COUNTER( [] () { __VA_ARGS__ }, __FILE__, __LINE__ )

        /**
           `TEST()` guts, do not use it directly.
        **/
        struct test_t {
            explicit test_t( std::function< void() > test, char const * file, int line );
        };

        /** Exception to throw if an assertion fails. **/
        struct assertion_failure_t: std::logic_error {
            explicit assertion_failure_t( string_t const & message ):
                std::logic_error( "Assertion failure: " + message )
                {};
        };

        /**
            Boolean check. The check passes if the given condition is evaluated to `true`, and
            fails otherwise.
        **/
        #define ASSERT( COND ) {                                                                   \
            if ( not ( COND ) ) {                                                                  \
                using error_t = tapper::test::assertion_failure_t;                                 \
                ERR( "( " << #COND << " ) is expected to be true but it is false." );              \
            };                                                                                     \
        }

        /**
            Comparison check. It compares two values, the check passes if the given values equal,
            and fails otherwise. In case of failure, the message includes stringified condition, as
            well as actula values of both `LHS` and `RHS`.

            @param LHS — The first value to compare (left-hand-side of `operator ==`).

            @param RHS — The second value to compare (right-hand-side of `operator ==`).
        **/
        #define ASSERT_EQ( LHS, RHS ) {                                                            \
            auto _lhs_ = ( LHS );                                                                  \
            auto _rhs_ = ( RHS );                                                                  \
            if ( not ( _lhs_ == _rhs_ ) ) {                                                        \
                using error_t = tapper::test::assertion_failure_t;                                 \
                ERR(                                                                               \
                    "( " << #LHS << " ) is expected to equal ( " << #RHS << " ) " <<               \
                    "but it does not; " <<                                                         \
                    "lhs is " << tapper::test::str( _lhs_ ) << "; " <<                             \
                    "rhs is " << tapper::test::str( _rhs_ ) << "."                                 \
                );                                                                                 \
            };                                                                                     \
        }

        /**
            Exception check. It runs the given piece of code, it is expected to throw an exception
            of the specified type. The check passes if exception of the specified type thrown, and
            its message matches the regular expression, and fails otherwise.

            @param CODE  — Code to run.

            @param TYPE  — Exception type which the code expected to throw.

            @param REGEX — String to build regular expression to match against the caught exception
            message.
        **/
        #define ASSERT_EX( CODE, TYPE, REGEX ) {                                                   \
            bool _caught_ = false;                                                                 \
            try {                                                                                  \
                CODE;                                                                              \
            } catch ( TYPE const & _ex_ ) {                                                        \
                _caught_ = true;                                                                   \
                auto _re_ = ( REGEX );                                                             \
                if ( not std::regex_match( _ex_.what(), std::regex( _re_ ) ) ) {                   \
                    using error_t = tapper::test::assertion_failure_t;                             \
                    ERR(                                                                           \
                        "{ " << #CODE << " } throws " << #TYPE << " " <<                           \
                        "but the message " << tapper::test::str( _ex_.what() ) << " " <<           \
                        "does not match " << tapper::test::str( _re_ ) << "."                      \
                    );                                                                             \
                };                                                                                 \
            } catch ( std::exception const & _ex_ ) {                                              \
                using error_t = tapper::test::assertion_failure_t;                                 \
                ERR(                                                                               \
                    "{ " << #CODE << " } is expected to throw " << #TYPE << " " <<                 \
                    "but throws " << typeid( _ex_ ).name() << " " <<                               \
                    "with message " << tapper::test::str( _ex_.what() ) << "."                     \
                );                                                                                 \
            };                                                                                     \
            if ( not _caught_ ) {                                                                  \
                using error_t = tapper::test::assertion_failure_t;                                 \
                ERR(                                                                               \
                    "{ " << #CODE << " } is expected to throw " << #TYPE << " " <<                 \
                    "but it does not throw."                                                       \
                );                                                                                 \
            };                                                                                     \
        }

        template< typename value_t >
        string_t str( value_t const & value ) { return tapper::str( value ); };

        template<>
        string_t str< char const * >( char const * const & value );

        template<>
        string_t str< string_t >( string_t const & value );

        template<>
        string_t str< strings_t >( strings_t const & value );

    }; // namespace test
    }; // namespace tapper

#else

    // No tests in production build.
    #define TEST( ... )

#endif // ENABLE_DEBUG

#endif // _TAPPER_TEST_HPP_

// end of file //
