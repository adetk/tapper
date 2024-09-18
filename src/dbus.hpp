/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/dbus.hpp

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
    TODO
**/

#ifndef _TAPPER_DBUS_HPP_
#define _TAPPER_DBUS_HPP_

#include <condition_variable>
#include <mutex>
#include <set>

#include <glibmm/refptr.h>
#include <glibmm/variant.h>

#include "base.hpp"

namespace Gio {
    namespace DBus {
        class Connection;
        class Proxy;
    };
};

#define CAST_DYNAMIC( TYPE, VALUE ) \
    Glib::VariantBase::cast_dynamic< Glib::TYPE >( VALUE )

namespace tapper {

class dbus_t: public object_t {

    public:

        class error_t: public std::runtime_error {

            public:

                using myself_t = error_t;
                using parent_t = std::runtime_error;

                enum code_t {
                    no_such_bus = 1,
                    bad_result_type,
                };

                using std::runtime_error::runtime_error;

                explicit error_t( string_t const & what, code_t code );
                code_t code() const;

            private:

                code_t _code;
        };

        dbus_t(
            string_t const & name,
            string_t const & path,
            string_t const & face
        );
        ~dbus_t();

        string_t name() const;
        string_t path() const;
        string_t face() const;

        Glib::VariantContainerBase
        call(
            string_t const &                    method,
            Glib::VariantContainerBase const &  args,
            string_t const &                    expected_result_type,
            std::chrono::milliseconds           timeout = std::chrono::milliseconds( 0 )
        );

    private:

        class thread_t;

        using mutex_t = std::mutex;
        using lock_t  = std::unique_lock< mutex_t >;
        using cvar_t  = std::condition_variable;

        using connection_t = Glib::RefPtr< Gio::DBus::Connection >;
        using proxy_t      = Glib::RefPtr< Gio::DBus::Proxy >;

    private:

        string_t const          _name;
        string_t const          _path;
        string_t const          _face;

        cvar_t                  _cvar;
        mutex_t                 _mutex;
        ptr_t< thread_t >       _thread;
        guint                   _id;
        connection_t            _conn;
        proxy_t                 _proxy;
        std::set< string_t >    _methods;

}; // class dbus_t

}; // namespace tapper

#endif // _TAPPER_XDG_HPP_

// end of file //
