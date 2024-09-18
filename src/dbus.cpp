/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/dbus.cpp

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

#include "dbus.hpp"

#include <atomic>

#include <glibmm/error.h>
#include <glibmm/main.h>

#include <giomm/dbusconnection.h>
#include <giomm/dbusproxy.h>
#include <giomm/dbuswatchname.h>

#include "posix.hpp"
#include "string.hpp"

namespace tapper {

// -------------------------------------------------------------------------------------------------
// dbus_t::error_t
// -------------------------------------------------------------------------------------------------

dbus_t::error_t::error_t(
    string_t const &    what,
    code_t              code
):
    parent_t( what ),
    _code( code )
{
};

dbus_t::error_t::code_t
dbus_t::error_t::code(
) const {
    return _code;
};

// -------------------------------------------------------------------------------------------------
// dbus_t::thread_t
// -------------------------------------------------------------------------------------------------

class dbus_t::thread_t: public posix::thread_t {
    public:
        using myself_t = thread_t;
        using parent_t = posix::thread_t;
        explicit thread_t():
            parent_t( "dbus" ),
            _main_loop( Glib::MainLoop::create() )
        {
        };
        void join() {
            _done = true;
            _main_loop->quit();
            parent_t::join();
        };
    protected:
        virtual void body() override {
            /*
                `_main_loop->run()` sometimes hangs, if `_main_loop->quit()` is called from another
                thread. It seems it is quite old problem:
                    <https://mail.gnome.org/archives/gtk-app-devel-list/2006-March/msg00073.html>
                Let's replace `run()` with manual loop.
                TODO: Write a test case, submit a bug against glib.
            */
            if ( 0 ) {
                _main_loop->run();
            } else {
                while ( ! _done ) {
                    _main_loop->get_context()->iteration( true );
                };
            };
        };
    private:
        std::atomic< bool >             _done { false };
        Glib::RefPtr< Glib::MainLoop >  _main_loop;
};

// -------------------------------------------------------------------------------------------------
// dbus_t
// -------------------------------------------------------------------------------------------------

dbus_t::dbus_t(
    string_t const & name,
    string_t const & path,
    string_t const & face
):
    OBJECT_T(),
    _name( name ),
    _path( path ),
    _face( face ),
    _thread( new thread_t() )
{
    TRACE();
    _thread->start();
    _id = Gio::DBus::watch_name(
        // Gio::DBus::BUS_TYPE_SESSION,
        Gio::DBus::BusType::SESSION,
        _name,
        [ this ](
            connection_t const & connection,
            string_t,
            string_t const &
        ) {
            lock_t  lock( _mutex );
            DBG( "Name " << q( _name ) << " appeared in the session bus." );
            _conn  = connection;
            _proxy = Gio::DBus::Proxy::create_sync( _conn, _name, _path, _face );
            _cvar.notify_all();
        },
        [ this ](
            connection_t const &,
            string_t
        ) {
            lock_t  lock( _mutex );
            DBG( "Name " << q( _name ) << " vanished in the session bus." );
            _proxy.reset();
            _conn.reset();
            _methods.clear();
        }
    );
};

dbus_t::~dbus_t(
) {
    TRACE();
    Gio::DBus::unwatch_name( _id );
    _thread->join();
};

string_t
dbus_t::name(
) const {
    return _name;
};

string_t
dbus_t::path(
) const {
    return _path;
};

string_t
dbus_t::face(
) const {
    return _face;
};

Glib::VariantContainerBase
dbus_t::call(
    string_t const &                    method,
    Glib::VariantContainerBase const &  args,
    string_t const &                    expected_result_type,
    std::chrono::milliseconds           timeout
) {
    TRACE();
    Glib::VariantContainerBase reply;
    lock_t lock( _mutex );
    auto ok = _cvar.wait_for(
        lock,
        timeout,
        [ this ] () {
            return !! _conn;
        }
    );
    if ( not ok ) {
        ERR( "There is no " << q( _name ) << " session bus.", error_t::no_such_bus );
    };
    DBG( "call: " << q( method ) << "." );
    reply = _proxy->call_sync( method, args );
    DBG( "repl: " << reply.print() );
    if ( _methods.find( method ) == _methods.end() ) {
        //~ try {
        //~ } catch ( Glib::Error const & ex ) {
            /*
                When GNOME Shell is restarted, it takes some time to restart all extensions.
                During this time method call may fail. Do not throw error if Gnome appeared
                less than few seconds ago.
            */
            //~ if ( ex.matches( G_DBUS_ERROR, G_DBUS_ERROR_UNKNOWN_METHOD ) ) {
                //~ WRN( ex.what() );
                //~ ok = false;
            //~ } else {
                //~ ERR( ex.what() );
            //~ };
        //~ } catch ( Glib::Exception const & ex ) {
            //~ ERR( ex.what() );
        //~ };
        string_t const actual_result_type = reply.get_type().get_string();
        auto eq = actual_result_type == expected_result_type;
        if ( not eq ) {
            ERR(
                q( method ) << " method "
                    << "is expected to return " << q( expected_result_type )
                    << " but actually it returned " << q( actual_result_type ),
                error_t::bad_result_type
            );
        };
        _methods.insert( method );
    };
    return reply;
};

}; // namespace tapper

// end of file //
