/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/posix.hpp

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
    POSIX wrappers interface.

    @sa posix.cpp
**/

#ifndef _TAPPER_POSIX_HPP_
#define _TAPPER_POSIX_HPP_

#include "base.hpp"

#include <initializer_list>
#include <stdexcept>    // std::runtime_error
#include <vector>

#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#if WITH_LIBCAP
    #include <sys/capability.h>
#endif // WITH_LIBCAP
#include <sys/stat.h>
#include <sys/types.h>

namespace tapper {
/// POSIX wrappers.
namespace posix {

    string_t get_env( string_t const & name, string_t dflt = "" );
    void     set_env( string_t const & name, string_t const & value );
    string_t get_home( uid_t uid = -1 );
    string_t base_name( string_t const & path );
    string_t syserrmsg( int error );
    void     execp( string_t const & prog, strings_t const & args );
    void     sleep();

    // ---------------------------------------------------------------------------------------------
    // error_t
    // ---------------------------------------------------------------------------------------------

    class error_t: public std::runtime_error {

        public:

            using std::runtime_error::runtime_error;
            explicit error_t( string_t const & what, int error );
            int error() const;

        private:

            int _error;

    }; // class error_t

    // ---------------------------------------------------------------------------------------------
    // Credentials
    // ---------------------------------------------------------------------------------------------

    uid_t constexpr no_uid = uid_t( -1 );
    gid_t constexpr no_gid = gid_t( -1 );

    using gids_t = std::vector< gid_t >;

    void   set_gid( gid_t gid );
    gids_t get_groups();
    void   set_groups( gids_t const & gids = gids_t() );

    template< typename id_t >
    struct _res_ids_t {
        id_t r;     ///< Real id.
        id_t e;     ///< Effective id.
        id_t s;     ///< Saved id.
        _res_ids_t( id_t _r, id_t _e, id_t _s ): r( _r ), e( _e ), s( _s ) {};
        _res_ids_t(): _res_ids_t( -1, -1, -1 ) {};
        bool operator ==( _res_ids_t const & that ) {
            return r == that.r and e == that.e and s == that.s;
        };
        string_t str() const {
            return
                string_t( "{ " )
                + "r:" + ( r == id_t( -1 ) ? "-" : tapper::str( r ) ) + ", "
                + "e:" + ( e == id_t( -1 ) ? "-" : tapper::str( e ) ) + ", "
                + "s:" + ( s == id_t( -1 ) ? "-" : tapper::str( s ) )
                + " }";
        };
    };

    using res_uids_t = _res_ids_t< uid_t >;
    using res_gids_t = _res_ids_t< gid_t >;

    res_uids_t get_user_ids();
    void       set_user_ids( res_uids_t const & ids );
    res_uids_t get_group_ids();
    void       set_group_ids( res_gids_t const & ids );

    uid_t get_user_id( string_t const & name );

    /** Returns gid for the given group name. Returns posix::no_gid if there is no such group. **/
    gid_t get_group_id( string_t const & name );

    // ---------------------------------------------------------------------------------------------
    // Capabilities
    // ---------------------------------------------------------------------------------------------

    #if WITH_LIBCAP

        class cap_t {

            public:

                enum class flag_t {
                    effective   = CAP_EFFECTIVE,
                    permitted   = CAP_PERMITTED,
                    inheritable = CAP_INHERITABLE,
                };

                enum class value_t {
                    setuid = CAP_SETUID,
                    setgid = CAP_SETGID,
                        // There are many other capability values, but I do not need them.
                };

                cap_t();
                cap_t( cap_t const & ) = delete;
                ~cap_t();

                cap_t & operator =( cap_t const & ) = delete;

                bool     get( flag_t flag, value_t value ) const;
                void     set( flag_t flag, value_t value, bool state );
                void     clear();
                void     apply();
                string_t str() const;

            private:

                static void free( void * ptr );

                ::cap_t  _rep;

        }; // class cap_t

    #endif // WITH_LIBCAP

    // ---------------------------------------------------------------------------------------------
    // File system
    // ---------------------------------------------------------------------------------------------

    using stat_t = struct stat;

    stat_t stat( string_t const & path );
    void   symlink( string_t const & target, string_t const & link );
    void   unlink( string_t const & path );
    void   mkdir( string_t const & path, mode_t mode = 0755 );

    class file_t {
        public:
            using myself_t = file_t;
            file_t() = default;
            file_t( myself_t const & that ) = delete;
            ~file_t();
            myself_t & operator =( myself_t const & that ) = delete;
            void open( string_t const & path, int flags = 0, int mode = 0 );
            void close();
            int fd();
        private:
            string_t _path;
            int      _fd = -1;
    };

    // ---------------------------------------------------------------------------------------------
    // signal_t
    // ---------------------------------------------------------------------------------------------

    /// Signal-related wrappers.
    namespace signal {

        /**
            Convenient operations on signal sets.
        **/
        struct set_t {
            explicit set_t();
            explicit set_t( std::initializer_list< int > const & list );
            void  clear();
            set_t & operator +=( int signo );
            sigset_t rep;
        }; // struct set_t

        struct action_t: sigaction {
            using handler_t = void ( * )( int );
            explicit action_t(
                handler_t     handler = nullptr,
                set_t const & mask    = set_t(),
                int           flags   = 0
            );
        };

        action_t action( int signal, action_t const & action );
        set_t    mask( int how, set_t const & set );
        int      wait( set_t const & set );
        void     kill( pid_t pid, int signal );

    }; // namespace signal

    // ---------------------------------------------------------------------------------------------
    // thread_t
    // ---------------------------------------------------------------------------------------------

    class thread_t: public object_t {

        public:

            explicit thread_t( string_t const name );
            virtual ~thread_t();

            void start();
            void join();
            void kill( int signal );

            #if ENABLE_DEBUG
                static string_t const * id();       ///< Reaturns id of the current thread.
            #endif // ENABLE_DEBUG

        protected:

            virtual void body() = 0;

        private:

            static void * _body( void * arg );

            enum state_t {
                inited,
                started,
                joined
            }; // enum state_t

            string_t        _name;
            state_t         _state;
            pthread_t       _handle;

        protected:

            string_t        _what;

    }; // class thread_t

}; // namespace posix

template<> string_t str( posix::gids_t const & gids );

}; // namespace tapper

#endif // _TAPPER_POSIX_HPP_

// end of file //
