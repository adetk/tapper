/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/base.cpp

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
    Tapper basic infrastructure implementation.
**/

#include "base.hpp"

#include <iostream>
#include <map>
#include <mutex>

#include <cstdlib>

#include "posix.hpp"
#include "range.hpp"
#include "string.hpp"
#include "test.hpp"

namespace tapper {

//  ------------------------------------------------------------------------------------------------
//  hex
//  ------------------------------------------------------------------------------------------------

TEST(
    ASSERT_EQ( hex( byte_t( 0x00 ) ), "00" );
    ASSERT_EQ( hex( byte_t( 0x01 ) ), "01" );
    ASSERT_EQ( hex( byte_t( 0x09 ) ), "09" );
    ASSERT_EQ( hex( byte_t( 0x0A ) ), "0A" );
    ASSERT_EQ( hex( byte_t( 0x0F ) ), "0F" );
    ASSERT_EQ( hex( byte_t( 0x10 ) ), "10" );
    ASSERT_EQ( hex( byte_t( 0xFF ) ), "FF" );

    ASSERT_EQ( hex( uint_t( 0x00000000 ) ), "00000000" );
    ASSERT_EQ( hex( uint_t( 0x00000001 ) ), "00000001" );
    ASSERT_EQ( hex( uint_t( 0x1F2EA3B4 ) ), "1F2EA3B4" );
    ASSERT_EQ( hex( uint_t( 0xFFFFFFFF ) ), "FFFFFFFF" );

    ASSERT_EQ( hex( int_t( 0x00000000 ) ), "00000000" );
    ASSERT_EQ( hex( int_t( 0xFFFFFFFF ) ), "FFFFFFFF" );
);

template< typename value_t >
value_t
_val(
    string_t const & string
) {
    using error_t = val_error_t;
    using limits_t = std::numeric_limits< value_t >;
    if ( limits_t::is_signed ) {
        if ( not like_integer( string ) ) {
            ERR( "Not an integer number." );
        };
    } else {
        if ( not like_cardinal( string ) ) {
            ERR( "Not a cardinal number." );
        };
    };
    assert( not string.empty() );
    auto    sign   = ( string[ 0 ] == '+' or string[ 0 ] == '-' ) ? string[ 0 ] : 0;
    value_t result = 0;
    for ( size_t i = sign ? 1 : 0, end = string.size(); i < end; ++ i ) {
        assert( '0' <= string[ i ] and string[ i ] <= '9' );
        int_t digit = string[ i ] - '0';
        if ( result > ( limits_t::max() - digit ) / 10 ) {
            t::range_t< value_t > range( limits_t::is_signed ? - limits_t::max() : 0 );
            ERR( "Out of range " << range << "." );
        };
        result = 10 * result + digit;
    };
    if ( sign == '-' ) {
        result = - result;
    };
    return result;
};

TEST(
    ASSERT_EQ( _val< int_t >(    "0" ),    0 );
    ASSERT_EQ( _val< int_t >(    "1" ),    1 );
    ASSERT_EQ( _val< int_t >( "+123" ),  123 );
    ASSERT_EQ( _val< int_t >( "-123" ), -123 );
);

template<>
int_t
val< int_t >(
    string_t const & string
) {
    TRACE();
    using error_t = val_error_t;
    try {
        return _val< int_t >( string );
    } catch ( error_t const & ex ) {
        ERR( "Bad integer number " << q( string ) << ": " << ex.what() );
    };
};

TEST(
    ASSERT_EQ( val< int_t >(           "0" ),           0 );
    ASSERT_EQ( val< int_t >(           "1" ),           1 );
    ASSERT_EQ( val< int_t >(          "12" ),          12 );
    ASSERT_EQ( val< int_t >(         "+23" ),          23 );
    ASSERT_EQ( val< int_t >(  "2147483647" ),  2147483647 );
    ASSERT_EQ( val< int_t >( "-2147483647" ), -2147483647 );
);

template<>
uint_t
val< uint_t >(
    string_t const & string
) {
    TRACE();
    using error_t = val_error_t;
    try {
        return _val< uint_t >( string );
    } catch ( error_t const & ex ) {
        ERR( "Bad cardinal number " << q( string ) << ": " << ex.what() );
    };
};

TEST(
    ASSERT_EQ( val< uint_t >(           "0" ),          0U );
    ASSERT_EQ( val< uint_t >(           "1" ),          1U );
    ASSERT_EQ( val< uint_t >(          "12" ),         12U );
    ASSERT_EQ( val< uint_t >(  "2147483647" ), 2147483647U );
    ASSERT_EQ( val< uint_t >(  "4294967295" ), 4294967295U );
);

// -------------------------------------------------------------------------------------------------
// object_t
// -------------------------------------------------------------------------------------------------

#if ENABLE_DEBUG

static thread_local uint_t _level = 0;

/**
    Generates object id from given object type name.
**/
static
string_t
gen_id(
    char const * type
        /**<
            Object type name. Since this function is called from class constructor, and constructor
            name equals to type (class) name, so it is expected the caller will pass `__FUNCTION__`
            to the constructor.
        **/
) {
    // Tapper does not have much contention in creating objects, so it is ok to use mutex.
    uint_t counter = 0;
    {
        static std::mutex mutex;
        static std::map< char const *, uint_t > counters;
        std::lock_guard< std::mutex > lock( mutex );
        counter = ++ counters[ type ];
    }
    return chop( type, "_t" ) + "#" + str( counter );
};

object_t::object_t(
    char const * type
):
    _id( gen_id( type ) )
{
}; // ctor

/** Returns constant `this` pointer. **/
object_t const *
object_t::_this_(
) const {
    return this;
}; // _this_

/**
    `_this_` global function, returns `nullptr`.

    The idea is that `_this_()` should be accessible everywhere. Inside a class derived from
    `object_t` `_this_()` is resulved to `objec_t::_this_()` method, outside of a class `_this_` is
    resolved to global function (this one), so `DBG()` macro can always call `_this()_` to find out
    object's id, if any.

    There are few more complications, though: static methods and functions outside of `tapper`
    namespace. Use `THIS()` macro to provide appropriate `_this_()` definition in these rare cases.
**/
object_t const *
_this_(
) {
    return nullptr;
}; // _this_

/**
    Returns constant reference to object's id. Object id is concatented object type (class) name
    (suffix `"_t"` common for all types, is dropped), hash character `#`, and object serial
    number.

    Object type is a string, passed to `object_t` constructor. Object serial number is assigned
    automatically. The first created instance of the class has serial number `1`, the next created
    instance has serial number `2`, and so on. Numbers of deleted objects are not reused, if object
    with serial number `2` is deleted, the next created instance will have serial number `3`
    (inspite the fact that serial number `2` is now free).

    For example, the id of the first instance of `display_t` will be `"display#1"`, the second
    instance will get id `"display#2"` and so on.

    Ids exists and used only in debug builds. Do not use them directly in the code.
**/
string_t const &
object_t::_id_(
) const {
    return _id;
}; // _id_

#endif // ENABLE_DEBUG

// -------------------------------------------------------------------------------------------------
// xprint functions
// -------------------------------------------------------------------------------------------------

/**
    Minimal priority level for sending messages to syslog. Messages with equal or higher priority
    will be send to syslog.
**/
static auto syslog_min_priority { priority_t( 0 ) };   /*
    Minimal Tapper priority, `priority_t::error`, is 3. Initialize `priority` with zero, no
    messages will be copied to syslog by default. Only call to `syslog_min_priority` allows
    copying.
*/

/**
    Returns minimal priority level for writing messages to `stderr` stream. Messages with equal or
    higher priority will be written to `stderr` stream.

    The function takes minimal priority level from `TAPPER_VERBOSITY` environment variable. I do
    use environment variable but not a command line option because Tapper starts printing messages
    before parsing the command line completed.

    The bigger value of `TAPPER_VERBOSITY` the more messages will be printed and vice versa.
    `TAPPER_VERBOSITY=8` enables trace messages. `TAPPER_VERBOSITY=0` blocks any messages,
    including errors. However, `TAPPER_VERBOSITY` does not affect messages sent to syslog. Also,
    currently command-line errors are not affected by `TAPPER_VERBOSITY`, since these messages are
    printed by `argp_parse()`.
**/
static
priority_t
stderr_min_priority(
) {
    static auto priority { priority_t( -1 ) };
    if ( int_t( priority ) < 0 ) {
        /*
            Do not use `val< int_t >` here! It can print messages, causing infinite loop.

            `posix::get_env` does not print messages, but I am afraid `string_t` (`Glib::ustring`)
            may throw an exception, so let's use dumb `atoi`.
        */
        auto const var = std::getenv( "TAPPER_VERBOSITY" );
        auto const num = std::atoi( var ? var : "" );
        if ( num > 0 ) {
            /*
                Avoid re-reading the env var if it contans negative value. Also, atoi returns 0 in
                case of failure (e. g. not a number), so treat zero as value absense and use
                default value instead.
            */
            priority = priority_t( num );
        } else {
            priority = priority_t::warning;
        };
    };
    return priority;
};

namespace _guts {

#if ENABLE_DEBUG

    /** Prints debug and trace messages. **/
    void
    dprint(
        object_t const *    obj,
            /**<
                `this` object, or `nullptr` if called not from an object. It is expected the caller
                will pass `_this_()`, which is specially dedicated for this purpose.
            **/
        char const *        file,
            /**< Source file name. It is expected the caller will pass `__FILE__`. **/
        int                 line,
            /**< Source file line number. It is expected the caller will pass `__LINE__`. **/
        priority_t          level,  ///< Priority level.
        string_t const &    mesg    ///< Message to print.
    ) {
        auto id = posix::thread_t::id();        // thread id looks like "thread#1".
        string_t no;                            // thread no is id without prefix "thread#".
        if ( id ) {                             // is is nullptr in main thread.
            no = id->substr( id->find_first_of( "#" ) + 1 );
        } else {
            no = "·";
        };
        eprint( level,
            "[" + no + "] " +
            pad_l( posix::base_name( file ), 25 ) + ":" + pad_r( str( line ), 4 ) +
            ":" +
            ( stderr_min_priority() >= priority_t::trace ? string_t( _level, ' ' ) : "" ) +
                // Indent messages only if tracing enabled.
            ( obj ? obj->_id_() + ": " : "" ) +
            mesg
        );
    }; // dprint

#endif // ENABLE_DEBUG

void eprint( priority_t priority, string_t const & message ) {
    string_t prefix = "?";
    switch ( priority ) {
        case priority_t::error: {
            prefix = "E";
        } break;
        case priority_t::warning: {
            prefix = "W";
        } break;
        case priority_t::info: {
            prefix = "I";
        } break;
        case priority_t::debug: {
            prefix = "#";
        } break;
        case priority_t::trace: {
            prefix = "⇄";
        } break;
    };
    if ( priority <= syslog_min_priority ) {
        ::syslog( int( priority ), "%s: %s", prefix.c_str(), message.c_str() );
    };
    if ( priority <= stderr_min_priority() ) {
        /*
            `std::cerr <<` is not used because it tends to throw exceptions if (1) the Tapper build
            `WITH_GLIB`, (2) text contains Unicode (non-ASCII) characters, (3) non-UTF-8 locale is
            in effect. Glibmm tries to convert Unicode string to non-Unicode encoding, fails and
            throws exception, which exact reason is hardly to find, since there is no any clue —
            neither file, nor line, nor even character.
        */
        auto buffer = STR(
            program_invocation_short_name << ": " << prefix << ": " << message << "\n"
        );
        std::cerr.write( DATA( buffer ) );
    };
}; // eprint

void oprint( string_t const & message ) {
    /*
        Do not use `std::cout <<`! See comment above.
    */
    auto buffer = message + "\n";
    std::cout.write( DATA( buffer ) );
}; // oprint

}; // namespace _guts

void set_syslog_min_priority( priority_t priority ) {
    assert( priority < priority_t::debug );
    syslog_min_priority = std::min( priority, priority_t::debug );
        // Never sent trace messages to syslog.
};

// -------------------------------------------------------------------------------------------------
// _tracer_t
// -------------------------------------------------------------------------------------------------

#if ENABLE_DEBUG

namespace _guts {

tracer_t::tracer_t(
    object_t const *    obj,
    char const *        file,
    int                 line,
    char const *        func
):
    _obj( obj ),
    _file( file ),
    _line( line ),
    _func( func )
{
    dprint( _obj, _file, _line, priority_t::trace, STR( func << " {" ) );
    ++ _level;
}; // ctor

tracer_t::~tracer_t(
) {
    -- _level;
    dprint( nullptr, _file, _line, priority_t::trace, "}" );
    //      ^^^^^^^ Do not prefix "}" with object id.
}; // dtor

}; // namespace _guts

#endif // ENABLE_DEBUG

}; // namespace tapper

// end of file //
