/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/posix.cpp

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
    POSIX wrappers implementation.
**/

#include "posix.hpp"

#include <limits>
#include <vector>

#include <errno.h>
#include <grp.h>
#include <libgen.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "string.hpp"
#include "test.hpp"

namespace tapper {
namespace posix {

/**
    Temporary read-write copy of `string_t` value.
**/
class buffer_t: private std::vector< char > {

    public:

        using  parent_t = std::vector< char >;

        using parent_t::parent_t;
        buffer_t(
            char const * str,   ///< Zero-terminated string to copy.
            size_t       size   ///< Number of bytes to copy (not include trailing zero).
        ): parent_t( str, str + size + 1 ) {};
        explicit buffer_t( string_t const & str ): buffer_t( DATA( str ) ) {};
        using parent_t::data;
        using parent_t::size;
        using parent_t::resize;

}; // class buffer_t

/**
    Returns value of an environment variable.

    It is C++ version of POSIX `getenv` — the function returns `string_t`, not `char *`. If the
    requested variable does not exist or empty, value of `dflt` string is returned.
**/
string_t
get_env(
    string_t const & name,  ///< Name of the environment variable to get.
    string_t         dflt   ///< Default value to return if the variable is not set or empty.
) {
    char const * value = getenv( name.c_str() );
    return ( value and value[ 0 ] ) ? value : dflt;
}; // get_env

void
set_env(
    string_t const & name,
    string_t const & value
) {
    auto err = setenv( name.c_str(), value.c_str(), 1 );
    if ( err ) {
        auto e = errno;
        ERR( "Can't set environment variable " << q( name ), e );
    };
};

/** Returns user home directory. **/
string_t
get_home(
    uid_t uid
) {
    auto home = get_env( "HOME" );
    if ( home.empty() ) {
        if ( uid < 0 ) {
            uid = get_user_ids().r;
        };
        buffer_t buffer( 1000 );
        for ( size_t attempt = 0; ; ++ attempt ) {
            struct passwd passwd;
            struct passwd * result = nullptr;
            auto err = getpwuid_r( uid, & passwd, buffer.data(), buffer.size(), & result );
            if ( err ) {
                int e = errno;
                if ( e == ERANGE and attempt < 10 ) {
                    buffer.resize( buffer.size() * 2 );
                } else {
                    ERR( "Can't get home directory of user " << uid, e );
                };
            } else {
                if ( result and result->pw_dir ) {
                    home = result->pw_dir;
                    break;
                };
            };
        };
    };
    return home;
}; // get_home

/**
    Returns file part of a path.

    It is C++ version of POSIX `basename` — the function does not change its argument and operates
    with `string_t`, not `char *`.
**/
string_t
base_name(
    string_t const & path
) {
    buffer_t buffer( path );
    return ::basename( buffer.data() );
}; // basename

TEST(
    ASSERT_EQ( base_name( "/dir/file.ext" ), "file.ext" );
    ASSERT_EQ( base_name( "/file.ext" ),     "file.ext" );
    ASSERT_EQ( base_name( "file.ext" ),      "file.ext" );
    ASSERT_EQ( base_name( "dir/" ),          "dir"      );
);

string_t
syserrmsg(
    int error
) {
    char buffer[ 1000 ];
    return strerror_r( error, buffer, sizeof( buffer ) );
}; // syserrmsg

void
execp(
    string_t const &  prog,
    strings_t const & args
) {
    /*
        `string_t::data()` in C++11 returns a constant string, but `execvp` requites non-constant
        string, so I have to make non-constant copy.
    */
    std::vector< buffer_t > buff;   // non-const copy of strings.
    std::vector< char * >   argv;   // argument vector.
    buff.reserve( args.size() + 1 );
    argv.reserve( args.size() + 2 );
    buff.emplace_back( prog );
    argv.emplace_back( buff.back().data() );
    for ( auto const & arg: args ) {
        buff.emplace_back( arg );
        argv.emplace_back( buff.back().data() );
    };
    argv.emplace_back( nullptr );
    auto err = execvp( prog.c_str(), argv.data() );
    if ( err ) {
        auto e = errno;
        ERR( "Can't execute " << q( prog ), e );
    };
};

void
symlink(
    string_t const & target,
    string_t const & link
) {
    auto err = ::symlink( target.c_str(), link.c_str() );
    if ( err ) {
        auto e = errno;
        ERR( "Can't create symlink " << q( link ), e );
    };
};

/** Sleep until interrupted. **/
void
sleep(
) {
    struct timespec time {
        .tv_sec = std::numeric_limits< ::time_t >::max(),
        .tv_nsec = 0,
    };
    for ( ; ; ) {
        auto error = nanosleep( & time, nullptr );
        if ( error ) {
            error = errno;
            if ( error == EINTR ) {
                break;
            };
            ERR( "nanosleep failed", error );
        };
    };
};

// -------------------------------------------------------------------------------------------------
// error_t
// -------------------------------------------------------------------------------------------------

error_t::error_t(
    string_t const &    what,
    int                 error
):
    std::runtime_error( STR( what << ": Error " << error << ": " << syserrmsg( error ) << "." ) ),
    _error( error )
{
}; // ctor

int error_t::error(
) const {
    return _error;
};

// -------------------------------------------------------------------------------------------------
// Credentials
// -------------------------------------------------------------------------------------------------

STATIC_ASSERT( not std::numeric_limits< uid_t >::is_signed );
STATIC_ASSERT( not std::numeric_limits< gid_t >::is_signed );
STATIC_ASSERT( sizeof( uid_t ) == sizeof( gid_t ) );

void
set_gid(
    gid_t gid
) {
    auto err = setgid( gid );
    if ( err ) {
        auto e = errno;
        ERR( "Setting group id failed", e );
    };
};

gids_t
get_groups(
) {
    gids_t gids( 32, no_gid );
    for ( size_t attempt = 0; ; ++ attempt ) {
        auto count = getgroups( gids.size(), gids.data() );
        if ( count < 0 ) {
            int e = errno;
            if ( e == EINVAL and attempt < 5 ) {
                gids.resize( gids.size() * 2 );
            } else {
                ERR( "Can't get groups", e );
            };
        } else {
            gids.resize( count );
            return gids;
        };
    };
};

void
set_groups(
    gids_t const & gids
) {
    auto err = setgroups( gids.size(), gids.data() );
    if ( err ) {
        auto e = errno;
        ERR( "Setting supplementary group ids failed", e );
    };
};

res_uids_t
get_user_ids(
) {
    res_uids_t ids;
    auto err = getresuid( & ids.r, & ids.e, & ids.s );
    if ( err ) {
        auto e = errno;
        ERR( "Getting user ids failed", e );
    };
    DBG( "Got user ids: " << ids << "." );
    return ids;
};

void
set_user_ids(
    res_uids_t const & ids
) {
    DBG( "Setting user ids: " << ids << "." );
    auto err = setresuid( ids.r, ids.e, ids.s );
    if ( err ) {
        auto e = errno;
        ERR( "Setting user ids failed", e );
    };
};

res_gids_t
get_group_ids(
) {
    res_gids_t ids;
    auto err = getresgid( & ids.r, & ids.e, & ids.s );
    if ( err ) {
        auto e = errno;
        ERR( "Getting group ids failed", e );
    };
    DBG( "Got group ids: " << ids );
    return ids;
};

void
set_group_ids(
    res_gids_t const & ids
) {
    DBG( "Setting group ids: " << ids << "." );
    auto err = setresgid( ids.r, ids.e, ids.s );
    if ( err ) {
        auto e = errno;
        ERR( "Setting group ids failed", e );
    };
};

uid_t
get_user_id(
    string_t const & name
) {
    struct passwd passwd;
    struct passwd * result = nullptr;
    buffer_t buffer( 1000 );
    for ( size_t attempt = 0; ; ++ attempt ) {
        auto err = getpwnam_r( name.c_str(), & passwd, buffer.data(), buffer.size(), & result );
        if ( err ) {
            int e = errno;
            if ( e == ERANGE and attempt < 10 ) {
                buffer.resize( buffer.size() * 2 );
            } else {
                ERR( "Can't get id of user " << q( name ), e );
            };
        } else {
            if ( not result ) {
                ERR( "Can't find id of user " << q( name ) << "." );
            };
            return result->pw_uid;
        };
    };
};

gid_t
get_group_id(
    string_t const & name
) {
    struct group group;
    struct group * result = nullptr;
    buffer_t buffer( 1000 );
    for ( size_t attempt = 0; ; ++ attempt ) {
        auto err = getgrnam_r( name.c_str(), & group, buffer.data(), buffer.size(), & result );
        if ( err ) {
            int e = errno;
            if ( e == ERANGE and attempt < 10 ) {
                buffer.resize( buffer.size() * 2 );
            } else {
                ERR( "Can't get id of group " << q( name ), e );
            };
        } else {
            if ( not result ) {
                ERR( "Can't find id of group " << q( name ) << "." );
            };
            return result->gr_gid;
        };
    };
};

// -------------------------------------------------------------------------------------------------
// Capabilities
// -------------------------------------------------------------------------------------------------

#if WITH_LIBCAP

    cap_t::cap_t(
    ) :
        _rep( cap_get_proc() )
    {
        if ( not _rep ) {
            int e = errno;
            ERR( "Can't get capabilities", e );
        };
    };

    bool
    cap_t::get(
        flag_t  flag,
        value_t value
    ) const {
        cap_flag_value_t result;
        auto err = cap_get_flag( _rep, cap_value_t( value ), cap_flag_t( flag ), & result );
        if ( err ) {
            int e = errno;
            ERR( "Can't get capability flag value", e );
        };
        return result == CAP_SET;
    };

    void
    cap_t::set(
        flag_t  flag,
        value_t value,
        bool    state
    ) {
        std::vector< cap_value_t > values = { cap_value_t( value ) };
        auto err =
            cap_set_flag(
                _rep,
                cap_flag_t( flag ),
                values.size(),
                values.data(),
                state ? CAP_SET : CAP_CLEAR
            );
        if ( err ) {
            int e = errno;
            ERR( "Can't set capability flag value", e );
        };
    };

    void
    cap_t::clear() {
        auto err = cap_clear( _rep );
        if ( err ) {
            int e = errno;
            ERR( "Can't clear capabilities", e );
        };
    };

    void
    cap_t::apply() {
        auto err = cap_set_proc( _rep );
        if ( err ) {
            int e = errno;
            ERR( "Can't apply capabilities", e );
        };
    };

    string_t
    cap_t::str(
    ) const {
        auto text = cap_to_text( _rep, nullptr );
        if ( not text ) {
            int e = errno;
            ERR( "Can't convert capabilities to text", e );
        };
        string_t str = text;
        free( text );
        return str;
    };

    void
    cap_t::free(
        void * ptr
    ) {
        auto err = cap_free( ptr );
        if ( err ) {
            int e = errno;
            ERR( "Can't free capabilities", e );
        };
    };

    cap_t::~cap_t() {
        CATCH_ALL( free( _rep ) );
    };

#endif // WITH_LIBCAP

// -------------------------------------------------------------------------------------------------
// File system
// -------------------------------------------------------------------------------------------------

stat_t
stat(
    string_t const & path
) {
    stat_t result;
    memset( & result, 0, sizeof( result ) );
    auto err = ::stat( path.c_str(), & result );
    if ( err ) {
        int e = errno;
        ERR( "Can't stat " << q( path ), e );
    };
    return result;
};

void
unlink(
    string_t const & path
) {
    auto err = ::unlink( path.c_str() );
    if ( err ) {
        auto e = errno;
        ERR( "Can't unlink " << q( path ), e );
    };
};

void
mkdir(
    string_t const & path,
    mode_t           mode
) {
    auto err = ::mkdir( path.c_str(), mode );
    if ( err ) {
        auto e = errno;
        ERR( "Can't mkdir " << q( path ), e );
    };
};

file_t::~file_t(
) {
    if ( _fd != -1 ) {
        CATCH_ALL( close() );
    };
};

void
file_t::open(
    string_t const & path,
    int              flags,
    int              mode
) {
    assert( _fd == -1 );
    _path = path;
    DBG( "Opening file " << q( _path ) << "…" );
    _fd = ::open( path.c_str(), flags, mode );
    if ( _fd == -1 ) {
        int e = errno;
        ERR( "Can't open file " << q( _path ), e );
    };
};

void
file_t::close(
) {
    DBG( "Closing file " << q( _path ) << "…" );
    auto err = ::close( _fd );
    if ( err ) {
        int e = errno;
        ERR( "Can't close file " << q( _path ), e );
    };
    _fd = -1;
};

int
file_t::fd(
) {
    return _fd;
};

// =================================================================================================
// signal
// =================================================================================================

namespace signal {

// -------------------------------------------------------------------------------------------------
// set_t
// -------------------------------------------------------------------------------------------------

/**
    Constructs empty signal set.
**/
set_t::set_t(
) {
    clear();
}; // ctor

set_t::set_t(
    std::initializer_list< int > const & list
):
    set_t()
{
    for ( auto signal: list ) {
        * this += signal;
    };
}; // ctor

/**
    Clears signal set. After invoking this methos signal set will be empty. Internally the method
    calls `sigemptyset` and checks error.
**/
void
set_t::clear(
) {
    int error = sigemptyset( & rep );
    if ( error ) {
        error = errno;
        ERR( "Clearing signal set failed", error );
    }; // if
}; // clear

/**
    Adds a signal to the signal set. Internally the method calls `sigaddset` and checks error.
**/
set_t &
set_t::operator +=(
    int signo
) {
    int error = sigaddset( & rep, signo );
    if ( error ) {
        error = errno;
        ERR( "Adding signal to signal set failed", error );
    }; // if
    return * this;
}; // +=

// -------------------------------------------------------------------------------------------------
// action_t
// -------------------------------------------------------------------------------------------------

action_t::action_t(
    handler_t     handler,
    set_t const & mask,
    int           flags
) {
    sa_handler  = handler;
    sa_mask     = mask.rep;
    sa_flags    = flags;
    sa_restorer = nullptr;
};

// -------------------------------------------------------------------------------------------------
// functions
// -------------------------------------------------------------------------------------------------

action_t
action(
    int              signal,
    action_t const & action
) {
    action_t old;
    auto error = ::sigaction( signal, & action, & old );
    if ( error ) {
        error = errno;
        ERR( "Setting signal mask failed", error );
    };
    return old;
};

/**
    Sets thread's mask of blocked signals.

    The function calls `pthread_sigmask` and throws an `error_t` exception in case of error.
**/
set_t
mask(
    int             how,
    set_t const &   set
) {
    set_t old;
    int error = pthread_sigmask( how, & set.rep, & old.rep );
    if ( error ) {
        ERR( "Setting signal mask failed", error );
    }; // if
    return old;
}; // mask

/**

**/
int
wait(
    set_t const &   set
) {
    TRACE();
    int signo = 0;
    int error = sigwait( & set.rep, & signo );
    if ( error ) {
        ERR( "Waiting for signal failed", error );
    }; // if
    return signo;
}; // wait

/**
    Sends a signal.

    The function calls POSIX `kill` and throws an `error_t` exception in case of error.
**/
void
kill(
    pid_t pid,  ///< Process to send signal to.
    int signal  ///< Signal to send.
) {
    int error = ::kill( pid, signal );
    if ( error ) {
        error = errno;
        ERR( "Sending signal failed", error );
    }; // if
}; // kill

}; // namespace signal

// -------------------------------------------------------------------------------------------------
// thread_t
// -------------------------------------------------------------------------------------------------

thread_t::thread_t(
    string_t const name
):
    OBJECT_T(),
    _name( name ),
    _state( inited )
{
    TRACE();
    DBG( "inited" );
}; // ctor

thread_t::~thread_t(
) {
    TRACE();
    if ( _state == started ) {
        CATCH_ALL( join() );
    }; // if
}; // dtor

void
thread_t::start(
) {
    TRACE();
    pthread_t thread;
    int error = pthread_create( & thread, nullptr, _body, this );
    if ( error ) {
        ERR( "Starting thread failed", error );
    }; // if
    #if ENABLE_DEBUG
        pthread_setname_np( thread, ( _id_() + "/" + _name ).c_str() );
    #endif // ENABLE_DEBUG
    DBG( "started" );
    _state  = started;
    _handle = thread;
}; // start

void
thread_t::join(
) {
    TRACE();
    switch ( _state ) {
        case inited: {
            DBG( "not started, nothing to do" );
        } break;
        case started: {
            void * ret = nullptr;
            int error = pthread_join( _handle, & ret );
            if ( error ) {
                ERR( "Joining thread failed", error );
            } else {
                DBG( "joined" );
            }; // if
            _state = joined;
            if ( not _what.empty() ) {
                DBG( "rethrowing exception occurred in thread body…" );
                ERR( _what );
            }; // if
        } break;
        case joined: {
            DBG( "already joined, nothing to do" );
        } break;
        default: {
            ERR( "Oops" );
        } break;
    }; // switch
}; // join

void
thread_t::kill(
    int signal
) {
    auto error = pthread_kill( _handle, signal );
    if ( error ) {
        ERR( "Killing thread failed", error );
    };
};

#if ENABLE_DEBUG

    /**
        Thread saved its id in this thread-local variable, so it is available everywhere without thread
        object (by calling `thread_t::id()`.
    **/
    static thread_local string_t const * _thread_id = nullptr;

    string_t const *
    thread_t::id(
    ) {
        return _thread_id;
    }; // id

#endif // ENABLE_DEBUG

void *
thread_t::_body(
    void * arg
) {
    thread_t * thread = reinterpret_cast< thread_t * >( arg );
    // cppcheck-suppress shadowFunction
    THIS( thread );
    #if ENABLE_DEBUG
        _thread_id = & thread->_id_();
    #endif // ENABLE_DEBUG
    TRACE();
    DBG( "really started" );
    auto what = CATCH_ALL( thread->body(); );
        /*
            Do not assign result of `CATCH_ALL` directly to `thread->_what`: it may overwrite error
            message leaved there by the thread body.
        */
    if ( not what.empty() ) {
        thread->_what = what;
    };
    return nullptr;
}; // _body

}; // namespace posix

template<>
string_t
str(
    posix::gids_t const & gids
) {
    if ( gids.empty() ) {
        return "{}";
    } else {
        strings_t groups;
        std::transform(
            gids.begin(), gids.end(),
            std::back_inserter( groups ),
            [] ( gid_t gid ) {
                return gid == posix::no_gid ? "-1" : str( gid );
            }
        );
        return "{ " + join( ", ", groups ) + " }";
    };
};


}; // namespace tapper

// end of file //
