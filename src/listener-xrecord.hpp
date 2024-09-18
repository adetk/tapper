/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/listener-xrecord.hpp

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
    `listener::xrecord_t` class interface.

    @sa listener-xrecord.cpp
**/

#ifndef _TAPPER_LISTENER_XRECORD_HPP_
#define _TAPPER_LISTENER_XRECORD_HPP_

#include "base.hpp"
#include "listener.hpp"

#include "x.hpp"    //  XRecordInterceptData

namespace tapper {
namespace listener {

// -------------------------------------------------------------------------------------------------
// xrecord_t
// -------------------------------------------------------------------------------------------------

/**
    XRecord listener. XRecord is an X Window System extension. This listener uses XRecord to listen
    user input events. The XRecord listener uses it and obviously requires X Window System and does
    not work in Wayland, but does not require extra permissions.
**/
class xrecord_t: public object_t, public listener_t {

    public:

        explicit xrecord_t();

        virtual string_t            type()                       override;
        virtual key_t::range_t      key_range()                  override;
        virtual keys_t              keys()                       override;
        virtual key_t               key( string_t const & name ) override;
        virtual string_t            key_name( key_t key )        override;
        virtual strings_t           key_names( key_t key )       override;

    protected:

        virtual void                _start()                     override;
        virtual void                _stop()                      override;

    private:

        using strings_p  = ptr_t< strings_t >;
        using key2name_t = std::map< key_t, string_t >;
        using key2name_p = ptr_t< key2name_t >;
        using name2key_t = std::map< string_t, key_t >;
        using name2key_p = ptr_t< name2key_t >;

        x::kb_t &          kb();
        x::kb_t::desc_t &  kb_desc();
        strings_t const &  key_names();
        key2name_t const & key2name();
        name2key_t const & name2key();

        void               on_intercept( XRecordInterceptData const * data );

        x::record::context_t    _context;
        x::kb_p                 _kb;
        x::kb_t::desc_p         _kb_desc;
        key_t::range_p          _key_range;
        strings_p               _key_names;
        key2name_p              _key2name;
        name2key_p              _name2key;

}; // class xrecord_t

}; // namespace listener
}; // namespace tapper

#endif // _TAPPER_LISTENER_XRECORD_HPP_

// end of file //
