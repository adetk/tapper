/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/base.hpp

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
    Basic infrastructure interface. Basic infrastructure provides basic types, facilities for error
    handling, logging, and debugging.

    @sa base.cpp
**/

#ifndef _TAPPER_BASE_HPP_
#define _TAPPER_BASE_HPP_

#include "config.h"

#include <exception>
#include <memory>           // std::unique_ptr
#include <stdexcept>        // std::invalid_argument
#include <vector>

#include <cassert>
#include <cstddef>          // size_t
#include <cstdint>          // int32_t, uint32_t

#include <syslog.h>         // LOG_ERR, LOG_WARNING, etc.

#if WITH_GLIB
    // #include <glibmm/exception.h>
    #include <glibmm/error.h>
    #include <glibmm/ustring.h>
#else
    namespace Glib {
        /**
            Fake Glib::Exception class to let code be compiled successfully without GLib.
        **/
        class Exception {
            public:
                Exception() = default;
                char const * what() const { return nullptr; };
        };
    };
    #include <string>
#endif // WITH_GLIB

#define STATIC_ASSERT( cond ) static_assert( cond, "oops" )

/// Top-level namespace.
namespace tapper {

/**
    @defgroup btypes  Basic types

    Basic types, not specific for tapper: various integers, strings, pointers, etc.
**/
/// @{

/** Unsigned 8-bit integer. **/
using byte_t = std::uint8_t;

/** 32-bit integer. **/
using int_t  = std::int32_t;

/** 32-bit unsigned integer. **/
using uint_t = std::uint32_t;

/** Unsigned integer type for array indexing and loop counting. **/
using std::size_t;

/** Shorter alias for `std::unique_ptr`. **/
template< typename value_t, typename deleter_t = std::default_delete< value_t > >
using ptr_t = std::unique_ptr< value_t, deleter_t >;

/**
    Shorter alias for frequently used type. Yeah, `string_t` is shorter than `std::string`. :-)
    Also, if Tapper is built with Glib, `string_t` is a synonym for `Glib::ustring`.

    @note `Glib::ustring::size` returns number of characters, while `std::string::size` returns
    number of bytes! To extract raw data from `string_t`, use `DATA()` macro.
**/
#if WITH_GLIB
    using string_t = Glib::ustring;
#else
    using string_t = std::string;
#endif // WITH_GLIB

/**
    Returns `string_t` raw content. Pay attention: the macro expands to *two* values: pointer to
    raw string content and size (number of bytes not including trailing zero) of that content.

    Example:

    @code
        auto buffer = STR( ... );
        std::cerr.write( DATA( buffer ) );
    @endcode
**/
#if WITH_GLIB
    #define DATA( STR ) (STR).c_str(), (STR).bytes()
#else
    #define DATA( STR ) (STR).c_str(), (STR).size()
#endif // WITH_GLIB

/**
    Character type. It could be either synonym for `char` (if Tapper built without Glib) or synonym
    for `gunichar` (if Tapper built with Glib). Use it everywhere you need a *character*, if you
    need a 8-bit integer, use `byte_t`.
**/
using char_t = string_t::value_type;

/** Shorter alias for frequently used constant. **/
auto constexpr npos = string_t::npos;

/** Shorter alias for frequently used type. **/
using strings_t = std::vector< string_t >;
using strings_p = ptr_t< strings_t >;

/// @}

template< typename value_t >
string_t hex( value_t value ) {
    static char const digits[] = "0123456789ABCDEF";
    STATIC_ASSERT( sizeof( digits ) == 16 + 1 );
    auto len = sizeof( value_t ) * 2;
    char result[ len + 1 ];
    result[ len ] = 0;
    for ( size_t i = 0; i < len; ++ i ) {
        result[ len - i - 1 ] = digits[ ( value >> ( i * 4 ) ) & 0x0F ];
    };
    return result;
};

/** @defgroup str Stringification

    Converting values (integers, enums, etc) to strings.
**/
/// @{

/**
    This is default implementation of `str` function: if argument is a class, and the class has
    `str` method, use it for stringification. If argument is not a class, or the class does not
    have `str` method, it is not considered as an error: the compiler will try either tem[late
    specializations or overloaded functions.
**/
template< typename class_t > inline string_t str( class_t const & value ) { return value.str(); };

template<> inline string_t str( string_t const & value ) { return value; };

inline string_t str( char const * value ) { return value; };
inline string_t str( char *       value ) { return value; };
inline string_t str( void const * value ) { return "0x" + hex( intptr_t( value ) ); };
inline string_t str( byte_t       value ) { return std::to_string( uint_t( value ) ); };

/*
    I can't simply define `str` for `int_t, `uint_t` and `size_t`, because on some systems `size_t`
    and `uint_t` are aliases for the same type. In such a case, compilation will fail because of
    `str` redefinition. Instead, let me define `str` for various `int` types:
*/
inline string_t str( int                    value ) { return std::to_string( value ); };
inline string_t str( unsigned int           value ) { return std::to_string( value ); };
inline string_t str( long int               value ) { return std::to_string( value ); };
inline string_t str( unsigned long int      value ) { return std::to_string( value ); };
inline string_t str( long long int          value ) { return std::to_string( value ); };
inline string_t str( unsigned long long int value ) { return std::to_string( value ); };
inline string_t str( double                 value ) { return std::to_string( value ); };

/**
    Internals, should not be used directly.
**/
namespace _guts {

    /**
        Trivial string buffer used by string builder `STR()`. Avoid using it directly. Use `STR()`
        macro instead.
    **/
    struct stream_t {

        stream_t & myself() { return * this; };

        /**
            This is the only `operator <<` in Tapper. Tapper uses `str` function for
            stringification, because a new definition of `str` function requires less typing than
            defining `operator <<`:

            @code
                string_t str( type_t value ) { return ...; };
                std::ostream & operator <<( std::ostream & stream, type_t value ) {
                    ...;
                    return stream;
                };
            @endcode
        **/
        template< typename value_t >
        stream_t &
        operator <<(
            value_t const &     value
        ) {
            buffer += str( value );
            return * this;
        };

        string_t buffer;

    }; // struct stream_t

}; // namespace _guts

/**
    Build a string. Items are stringified with `str()` function.

    Usage:

    @code
        auto s = STR( "string literal" << number << whatever_else );
    @endcode

    It is a bit less boring than straightforward:

    @code
        auto s = str( "string literal" ) + str( number ) + str( whatever_else );
    @endcode
**/
#define STR( ARGS ) \
    ( tapper::_guts::stream_t().myself() << ARGS ).buffer

/// @}

/**
    @defgroup val Valuefication

    Converting strings back to values (integers, enums, etc).
**/
/// @{

struct val_error_t: std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};

/**
    Generic template function for valuefication. It does not have any implementation. All specific
    functions will be added as specializations of the template function.

    Usage:

    @code
        auto i = val< int_t >( "100" );
        assert( i == 100 );
    @endcode
**/
template< typename value_t >
value_t
val(
    string_t const & string
);

/// @}

/**
    Base class for "heavy" Tapper classes.

    "Heavy" objects are not copyable: copy constructor and assignment operator are deleted.

    `object_t` provides debugging facilities for its descendants: each object has an id (see
    `_id_()`)

    It is expected `OBJECT_T()` is used for calling `object_t` constructor.

    All these stuff is used for debugging purposes solely in debug builds. In prodiction
    (non-debug) build the class has no methods, just default constructor and desctructor.

**/
class object_t {

    // Objects are non-copyable.
    object_t( object_t const & ) = delete;
    object_t & operator =( object_t const & ) = delete;

    #if ENABLE_DEBUG

        public:

            explicit object_t( char const * type );
            virtual ~object_t() = default;

            object_t const * _this_() const;
            string_t const & _id_()   const;

        private:

            string_t const  _id;    ///< Object id, unique within the program.

    #else

        public:

            explicit object_t() = default;
            virtual ~object_t() = default;

    #endif // ENABLE_DEBUG

}; // object_t

/**
    Calls `object_t` constructor. It is used to handle difference between debug and production
    builds: in debug build the macro passes `__FUNCTION__` to the `object_t` constructor, in
    production build it calls `object_t` constructor with no arguments.

    Usage:

    @code
        class something_t: public object_t {
            public:
                something_t::something_t(): OBJECT_T() {
                    ...
                };
                ...
        };
    @endcode
**/
#if ENABLE_DEBUG
    #define OBJECT_T()  object_t( __FUNCTION__ )
#else
    #define OBJECT_T()  object_t()
#endif

/** Do not use it directly. **/
#if ENABLE_DEBUG
    object_t const * _this_();
#endif

/**
    Provides `_this_` definition for static methods.

    `DBG()` rely on `_this_()`, which could be either `object_t` method or global function.
    However, it does not work in static methods since `_this_` method is defined but not
    accessible. `THIS()` macro provides `_this_` definition for static methods:

    @code
        void a_static_method() {
            THIS( pointer-to-this-object ); // pointer-to-this-object may be nullptr.
            ...
            ERR( "message" );
        };
    @endcode
**/
#define THIS( OBJ ) auto _this_ = [ = ] () { return OBJ; }; static_cast< void >( _this_ );

/**
    Message priority level.

    These priority levels (mostly) taken from syslog priority levels.

    @note The higher priority, the less priority numerical value. E. g. `error < debug`.
**/
enum class priority_t {
    error   = LOG_ERR,      // 3
    warning = LOG_WARNING,  // 4
    info    = LOG_INFO,     // 6 (LOG_NOTICE is not used)
    debug   = LOG_DEBUG,    // 7
    trace   = debug + 1     // 8
}; // enum priority_t

namespace _guts {

#if ENABLE_DEBUG
    /// This is `DBG()` guts. Do not use it directly. Use `DBG()` and `TRACE()` instead.
    void dprint(
        object_t const * obj,
        char const *     file,
        int              line,
        priority_t       priority,
        string_t const & mesg
    );
#endif

/// `INF()`, `WRN()`, and `ERR()` guts. Do not use it directly.
void eprint( priority_t priority, string_t const & message );

/// `OUT()` guts. Do not use it directly.
void oprint( string_t const & message );

}; // namespace _guts

/**
    Sets minimal priority level for sending messages to syslog. Messages with the given priority
    and higher will be send to syslog. Trace messages are never send to syslog, though.
**/
void set_syslog_min_priority( priority_t priority );

/**
    Prints debug message.

    Usage:
    @code
        DBG( "Opening file " << q( path ) << "..." );
    @endcode

    In case of debug build, the result includes the program name, thread number, source location
    (file name and line number), and object (type and number) the message is printed from. For
    example, the statement above prints:

    @verbatim
    tapper: # [·]       li.cpp:199 :context#1: Opening file ‘/dev/input/event1’...
    @endverbatim

    where `tapper` is the program name, `#` is the message priority (`#` stands for debug, `W` for
    warning, `E` for error), `[·]` is thread number (dot denotes primary thread, other threads will
    be numbered starting from 1), `li.cpp:199` is the file name and line number, `context#1` is
    type and serial number of object, the rest is the `DBG()` arguments.

    In case of production build the macro prints no message (and produces no code).

    If `DBG()` is called from static method, use `THIS()` macro before calling `DBG()`, otherwise
    the compiler will generate error:

    @verbatim
    file.cpp:156:18: error: cannot call member function ‘const tapper::object_t*
        tapper::object_t::_this_() const’ without object
    @endverbatim

**/
#define DBG( ARGS ) \
    DBG_( _this_(), ARGS )

/// `DBG()` guts. Do not use it directly, use `DBG()` instead.
#if ENABLE_DEBUG
    #define DBG_( OBJ, ARGS ) \
        tapper::_guts::dprint( OBJ, __FILE__, __LINE__, tapper::priority_t::debug, STR( ARGS ) )
#else
    #define DBG_( OBJ, ARGS ) void( 0 )
#endif

/**
    Prints informational message.
**/
#define INF( ARGS ) \
    INF_( _this_(), ARGS )

/// `INF()` guts. Do not use it directly, use `INF()` instead.
#if ENABLE_DEBUG
    #define INF_( OBJ, ARGS )                                                                      \
        tapper::_guts::dprint( OBJ, __FILE__, __LINE__, tapper::priority_t::info, STR( ARGS ) )
#else
    #define INF_( OBJ, ARGS )                                                                      \
        tapper::_guts::eprint( priority_t::info, STR( ARGS ) )
#endif // ENABLE_DEBUG

/**
    Prints warning message.
**/
#define WRN( ARGS ) \
    WRN_( _this_(), ARGS )

/// `WRN()` guts. Do not use it directly, use `WRN()` instead.
#if ENABLE_DEBUG
    #define WRN_( OBJ, ARGS )                                                                      \
        tapper::_guts::dprint( OBJ, __FILE__, __LINE__, tapper::priority_t::warning, STR( ARGS ) )
#else
    #define WRN_( OBJ, ARGS )                                                                      \
        tapper::_guts::eprint( priority_t::warning, STR( ARGS ) )
#endif // ENABLE_DEBUG

/**
    Constructs a message from `ARGS` argument and throws an exception of `error_t` class. In case
    of debug build, it prints debug message using `DBG` macro just before throwing the exception.

    @code
        ERR( "Out of range " << range << "." );
    @endcode
**/
#define ERR( ARGS, ... ) \
    ERR_( _this_(), ARGS, ## __VA_ARGS__ )

/// `ERR()` guts. Do not use it directly, use `ERR()` instead.
#define ERR_( OBJ, ARGS, ... ) {                                                                   \
    auto _err_ = error_t( STR( ARGS ), ## __VA_ARGS__ );                                           \
    DBG_( OBJ, "Throwing: " << _err_.what() );                                                     \
    throw _err_;                                                                                   \
}

/**
    Prints message to standard output. Use it for normal, not debug messages.
**/
#define OUT( ARGS ) \
    tapper::_guts::oprint( STR( ARGS ) )

/**
    Catches all exceptions, prints a warning if an exception was caught, return `what()` of the
    caught exception or empty string if no exception was caught.

    The macro is used primarily in destructors:

    @code
        something_t::~something_t(
        ) {
            CATCH_ALL( close() );   // Avoid throwing exceptions from destructor.
        };
    @endcode
**/
#define CATCH_ALL( STATEMENT ) \
    CATCH_ALL_( _this_(), STATEMENT )

/// `CATCH_ALL()` guts. Do not use it directly, use `CATCH_ALL()` instead.
#define CATCH_ALL_( OBJ, STATEMENT ) ( {                                                           \
    string_t what;                                                                                 \
    try {                                                                                          \
        STATEMENT;                                                                                 \
    } catch ( Glib::Error const & ex ) {                                                       \
        what = ex.what();                                                                          \
    } catch ( std::exception const & ex ) {                                                        \
        what = ex.what();                                                                          \
    } catch ( ... ) {                                                                              \
        what = "(Unknown exception)";                                                              \
    };                                                                                             \
    if ( not what.empty() ) {                                                                      \
        WRN_( OBJ, "Exception caught: " << what );                                                 \
    };                                                                                             \
    what;                                                                                          \
} )

/**
    Traces entering and leaving a scope.

    Usage:

    @code
        void
        function_or_method(
        ) {
            TRACE();
            ...
        };
    @endcode

    The example above in case of debug build will print something like this:

    @verbatim
    tapper: # [·]        file.cpp:165:function_or_method {
    ...
    tapper: # [·]        file.cpp:165:}
    @endverbatim

    Left brace '`{`' denotes entering a scope, right brace '`}`' — leaving a scope. All the debug
    messages printed in the scope and nested traces will be indented accordingly to the nesting
    level.

    In prodiction build macro prints noting (and produces no code).
**/
#define TRACE() \
    TRACE_( _this_() )

/// `TRACE()` guts, do not use it directly, use `TRACE()` instead.
#if ENABLE_DEBUG
    #define TRACE_( OBJ ) \
        tapper::_guts::tracer_t _tracer_ ## __COUNTER__( OBJ, __FILE__, __LINE__, __FUNCTION__ )
#else
    #define TRACE_( OBJ )
#endif

#if ENABLE_DEBUG

    namespace _guts {

        /// `TRACE()` guts, do not use it directly.
        class tracer_t {
            public:
                tracer_t( object_t const * obj, char const * file, int line, char const * func );
                ~tracer_t();
            private:
                object_t const * const  _obj;
                char const * const      _file;
                int const               _line;
                char const * const      _func;
        }; // class _func_tracer_t

    }; // namespace _guts

#endif

}; // namespace tapper

#endif // _TAPPER_BASE_HPP_

// end of file //
