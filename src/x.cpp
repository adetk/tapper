/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/x.cpp

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
    X Window System wrappers implementation.
**/

#include "x.hpp"

#include <limits>

#include <unistd.h>

#include <X11/extensions/XTest.h>

#include "linux.hpp"
#include "string.hpp"
#include "test.hpp"

namespace tapper {

template<>
inline
string_t
str( Display * const & value ) {
    return str( static_cast< void const * >( value ) );
};

namespace x {

static XErrorHandler _standard_error_handler = nullptr;

static
int
error_handler(
    Display *       display,
    XErrorEvent *   event
) {
    #if 1
        char error_text[ 1000 ];
        XGetErrorText( event->display, event->error_code, error_text, sizeof( error_text ) );
        // TODO: How to decode opcode?
        // If I could decode opcode, I would not call standard error handler.
        ERR(
            "Display " << display << " reports error:\n"
            "    Display connection : " << XConnectionNumber( event->display ) << "\n"
            "    Request number     : " << event->serial << "\n"
            "    Opcode             : " << event->request_code << "." << event->minor_code << "\n"
            "    Error              : " << event->error_code << " (" << error_text << ")\n"
        );
    #endif
    #if 1
        if ( _standard_error_handler ) {
            _standard_error_handler( display, event );
        }; // if
    #endif
    return 0;
}; // error_handler

// -------------------------------------------------------------------------------------------------
// key_t
// -------------------------------------------------------------------------------------------------

key_t::key_t(
    rep_t code
): parent_t( code ) {
    if ( code ) {
        range_t().check( * this );
    };
    assert( not _code or is_valid() );
};

TEST(
    ASSERT_EQ( key_t::none, 0 );
    ASSERT_EQ( key_t().code(), key_t::none );
    ASSERT_EX( key_t( 1 ), val_error_t, R"(Out of range \[8…255\]\.)" );
    ASSERT_EX( key_t( 7 ), val_error_t, R"(Out of range \[8…255\]\.)" );
    ASSERT_EQ( key_t( 8 ).code(), 8U );
    ASSERT_EQ( key_t( 9 ).code(), 9U );
    ASSERT_EQ( key_t( 255 ).code(), 255U );
);

key_t::key_t(
    linux::key_t key
) {
    using error_t = val_error_t;
    if ( key.code() <= max - offset ) {
        _code = key.code() + offset;
    } else {
        ERR( "Linux key " << key << " can't be converted to X key." );
    };
    assert( not _code or is_valid() );
};

TEST(
    ASSERT_EQ( key_t( linux::key_t(   0 ) ).code(),   8 );
    ASSERT_EQ( key_t( linux::key_t(   1 ) ).code(),   9 );
    ASSERT_EQ( key_t( linux::key_t( 247 ) ).code(), 255 );
    ASSERT_EX(
        key_t( linux::key_t( 248 ) ),
        val_error_t,
        R"(Linux key 248 can't be converted to X key\.)"
    );
);

linux::key_t
key_t::linux(
) const {
    assert( not _code or is_valid() );
    return linux::key_t( _code ? _code - offset : none );
};

TEST(
    ASSERT_EQ( key_t().linux().code(), linux::key_t::none );
    ASSERT_EQ( key_t(   8 ).linux().code(),   0U );
    ASSERT_EQ( key_t(   9 ).linux().code(),   1U );
    ASSERT_EQ( key_t( 255 ).linux().code(), 247U );
);

// -------------------------------------------------------------------------------------------------
// btn_t
// -------------------------------------------------------------------------------------------------

btn_t::btn_t(
    rep_t code
): parent_t( code ) {
    if ( code ) {
        range_t().check( * this );
    };
    assert( not _code or is_valid() );
};

TEST(
    ASSERT_EQ( btn_t::none, 0U );
    ASSERT_EQ( btn_t().code(), btn_t::none );
    ASSERT_EQ( btn_t( btn_t::left ).code(), btn_t::left );
    ASSERT_EQ( btn_t( btn_t::max  ).code(), btn_t::max );
    ASSERT_EX( btn_t( btn_t::max + 1 ), val_error_t, R"(Out of range \[1…20\]\.)" );
);

btn_t::btn_t(
    linux::key_t key
) {
    if ( key.is_mouse_button() ) {
        switch ( key.code() ) {
            case linux::key_t::btn_left: {
                _code = left;
            } break;
            case linux::key_t::btn_right: {
                _code = right;
            } break;
            case linux::key_t::btn_middle: {
                _code = middle;
            } break;
            default: {
                assert( linux::key_t::btn_fourth <= key.code() );
                assert( key.code() <= linux::key_t::btn_max );
                _code = fourth + ( key.code() - linux::key_t::btn_fourth );
            } break;
        };
    } else {
        using error_t = val_error_t;
        ERR( "Linux key " << key << " can't be converted to X button." );
    };
    assert( not _code or is_valid() );
};

TEST(
    ASSERT_EQ( btn_t( linux::key_t( linux::key_t::btn_left       ) ).code(), btn_t::left       );
    ASSERT_EQ( btn_t( linux::key_t( linux::key_t::btn_middle     ) ).code(), btn_t::middle     );
    ASSERT_EQ( btn_t( linux::key_t( linux::key_t::btn_right      ) ).code(), btn_t::right      );
    ASSERT_EQ( btn_t( linux::key_t( linux::key_t::btn_fourth     ) ).code(), btn_t::fourth     );
    ASSERT_EQ( btn_t( linux::key_t( linux::key_t::btn_fourth + 1 ) ).code(), btn_t::fourth + 1 );
    ASSERT_EQ( btn_t( linux::key_t( linux::key_t::btn_max        ) ).code(), btn_t::max        );
    ASSERT_EX(
        btn_t( linux::key_t( 1 ) ),
        val_error_t,
        R"(Linux key 1 can't be converted to X button\.)"
    );
    ASSERT_EX(
        btn_t( linux::key_t( linux::key_t::btn_min - 1 ) ),
        val_error_t,
        R"(Linux key \d+ can't be converted to X button\.)"
    );
    ASSERT_EX(
        btn_t( linux::key_t( linux::key_t::btn_max + 1 ) ),
        val_error_t,
        R"(Linux key \d+ can't be converted to X button\.)"
    );
);

/*
    TODO: Use XGetPointerMapping?
*/
linux::key_t
btn_t::linux(
) const {
    assert( not _code or is_valid() );
    linux::key_t result;
    switch ( _code ) {
        case none: {
            // Do nothing.
        } break;
        case left: {
            result = linux::key_t( linux::key_t::btn_left );
        } break;
        case middle: {
            result = linux::key_t( linux::key_t::btn_middle );
        } break;
        case right: {
            result = linux::key_t( linux::key_t::btn_right );
        } break;
        case wheel_min ... wheel_max: {
            // Do nothing.
        } break;
        case fourth ... max: {
            result = linux::key_t( linux::key_t::btn_fourth + ( _code - fourth ) );
        } break;
        default: {
            assert( 0 );
        };
    };
    assert( not result.code() or result.is_mouse_button() );
    return result;
};

TEST(
    ASSERT_EQ( btn_t( btn_t::left   ).linux().code(), linux::key_t::btn_left   );
    ASSERT_EQ( btn_t( btn_t::right  ).linux().code(), linux::key_t::btn_right  );
    ASSERT_EQ( btn_t( btn_t::middle ).linux().code(), linux::key_t::btn_middle );
    for ( size_t b = btn_t::fourth; b <= btn_t::max; ++ b ) {
        ASSERT_EQ( btn_t( b ).linux().code(), linux::key_t::btn_fourth + b - btn_t::fourth );
    };
    ASSERT_EQ( btn_t( btn_t::fourth ).linux().code(), linux::key_t::btn_fourth );
    ASSERT_EQ( btn_t( btn_t::max    ).linux().code(), linux::key_t::btn_max    );
    for ( size_t b = btn_t::wheel_min; b <= btn_t::wheel_max; ++ b ) {
        ASSERT_EQ( btn_t( b ).linux().code(), linux::key_t::none );
    };
);

void
init(
) {
    Status ok = XInitThreads();
    if ( not ok ) {
        ERR( "Initializing X failed" );
    }; // if
    _standard_error_handler = XSetErrorHandler( error_handler );
    DBG( "error handler installed" );
}; // init

// -------------------------------------------------------------------------------------------------
// display_t
// -------------------------------------------------------------------------------------------------

display_t::display_t(
):
    OBJECT_T()
{
    TRACE();
    _rep = XOpenDisplay( nullptr );
    if ( not _rep ) {
        ERR( "Opening display " << q( XDisplayName( nullptr ) ) << " failed." );
    }; // if
    DBG(
        "opened, "
            << "name " << q( XDisplayString( _rep ) ) << ", "
            << "connection " << connection()
    );
}; // ctor

display_t::~display_t(
) {
    TRACE();
    if ( _rep ) {
        XCloseDisplay( _rep );  /*
            XClose has `int` result. It is not documented, but sources are available and I see that
            `XCloseDisplay` always returns zero.
        */
        _rep = nullptr;
        DBG( "closed" );
    }; // if
}; // dtor

display_t::operator Display *(
) const {
    assert( _rep );
    return _rep;
};

int
display_t::connection(
) const {
    return XConnectionNumber( _rep );
}; // connection

void
display_t::flush(
) {
    Status ok = XFlush( _rep );
    if ( not ok ) {
        ERR( "Flushing display failed." );
    }; // if
}; // flush

void
display_t::sync(
    bool discard
) {
    Status ok = XSync( _rep, discard );
    if ( not ok ) {
        ERR( "Synchronizing display failed." );
    }; // if
}; // sync

// -------------------------------------------------------------------------------------------------
// atom_t
// -------------------------------------------------------------------------------------------------

atom_t::atom_t(
    display_t const & display,
    Atom              atom
):
    OBJECT_T(),
    _display( display ),
    _rep( atom )
{
}; // ctor

string_t
atom_t::name(
) {
    char * name = XGetAtomName( _display, _rep );
    // cppcheck-suppress nullPointerRedundantCheck
    if ( not name ) {
        ERR( "Getting atom name failed." );
    }; // if
    string_t res = name;
    XFree( name );
    return res;
}; // name

// =================================================================================================
// record
// =================================================================================================

/*
    Primary documentation:
    http://www.x.org/releases/X11R7.7/doc/libXtst/recordlib.html
*/

namespace record {

// -------------------------------------------------------------------------------------------------
// intercept_data_t
// -------------------------------------------------------------------------------------------------

intercept_data_t::intercept_data_t(
    XRecordInterceptData * _data
):
    OBJECT_T(),
    data( _data )
{
}; // ctor

intercept_data_t::~intercept_data_t(
) {
    if ( data ) {
        XRecordFreeData( data ); // void function
        data = nullptr;
    }; // if
}; // dtor

// -------------------------------------------------------------------------------------------------
// range_t
// -------------------------------------------------------------------------------------------------

range_t::range_t(
    unsigned char first,
    unsigned char last
):
    OBJECT_T()
{
    _rep = XRecordAllocRange();
    if ( not _rep ) {
        ERR( "Allocating record range failed." );
    }; // if
    _rep->device_events.first = first;
    _rep->device_events.last  = last;
    DBG( "allocated" );
}; // ctor

range_t::~range_t() {
    if ( _rep ) {
        Status ok = XFree( _rep );
        if ( not ok ) {
            WRN( "Freeing record range failed." );
        }; // if
        DBG( "freed" );
        _rep = nullptr;
    }; // if
}; // dtor

// -------------------------------------------------------------------------------------------------
// context_t
// -------------------------------------------------------------------------------------------------

context_t::context_t(
    on_intercept_t      on_intercept,
    int                 datum_flags,
    XRecordClientSpec   client
):
    OBJECT_T(),
    _on_intercept( on_intercept ),
    _thread( _data )
{

    TRACE();

    // TODO: Move version check into a separate function?
    // TODO: Check version?
    int major = 0;
    int minor = 0;
    Status ok = XRecordQueryVersion( _ctrl, & major, & minor );
    if ( not ok ) {
        ERR( "Querying XRecord extension version failed." );
    }; // if
    DBG( "XRecord extension version " << major << "." << minor << "." );

    range_t         keys(       KeyPress,    KeyRelease );
    range_t         buttons( ButtonPress, ButtonRelease );
    XRecordRange *  ranges[] = { keys._rep, buttons._rep };
    _handle = XRecordCreateContext(
        _ctrl,
            /*
                This request is typically executed by the recording client over its control
                connection to the X server.
                http://www.x.org/releases/X11R7.7/doc/libXtst/recordlib.html#XRecordCreateContext
            */
        datum_flags,
        & client,
        1,          // Number of clients.
        ranges,
        sizeof( ranges ) / sizeof( XRecordRange * )     // Number of ranges.
    );
    if ( _handle == None ) {
        ERR( "Creating record context failed." );
    }; // if
    DBG( "created" );

}; // ctor

context_t::~context_t(
) {
    TRACE();
    if ( _handle != None ) {
        if ( _state == enabled ) {
            CATCH_ALL( disable() );
        }; // if
        Status ok = XRecordFreeContext( _ctrl, _handle );
        if ( not ok ) {
            WRN( "Freeing record context failed." );
        }; // if
        _handle = None;
        DBG( "freed" );
    };
}; // dtor

display_t &
context_t::display(
) {
    return _ctrl;
};

void
context_t::enable(
) {
    TRACE();
    _ctrl.sync();
    /*
        Sync is a must. Otherwise XRecordEnableContextAsync will likely fail with error
        XRecordBadContext.
    */
    Status ok = XRecordEnableContextAsync(
        _data,
        _handle,
        interceptor,
        reinterpret_cast< XPointer >( this )
    );
    if ( not ok ) {
        ERR( "Enabling record context failed." );
    }; // if
    /*
        My experiments show interceptor is called once with event XRecordStartOfData from within
        XRecordEnableContextAsync, so context should be enabled now.
    */
    if ( _state != enabled ) {
        ERR( "Record context is not really enabled." );
    }; // if
    _thread.start();
}; // enable

void
context_t::disable(
) {
    TRACE();
    switch ( _state ) {
        case inited: {
            DBG( "not enabled, nothing do do" );
        } break;
        case enabled: {
            /*
                To direct the X server to halt the reporting of recorded protocol, the
                program executes XRecordDisableContext typically over its **control**
                connection to the X server.

                The XRecordDisableContext function disables context, stopping all
                recording over its data connection. Any complete protocol elements for
                context that were buffered in the server will be sent to the recording
                client rather than being discarded. If a program attempts to disable an
                XRecordContext that has not been enabled, no action will take place.

                http://www.x.org/releases/X11R7.7/doc/libXtst/recordlib.html#Disable_Context
            */
            Status ok = XRecordDisableContext( _ctrl, _handle );
            if ( ok ) {
                DBG( "disabled" );
            } else {
                ERR( "Disabling record context failed." );
            }; // if
            _ctrl.sync();   // Sync is a must.
            _thread.join();
            if ( _state != disabled ) {
                ERR( "Record context is not really disabled." );
            }; // if
        } break;
        case disabled: {
            DBG( "already disabled, nothing do do" );
        } break;
        default: {
            ERR( "Oops" );
        } break;
    }; // switch
}; // disable

void
context_t::interceptor(
    XPointer                closure,
    XRecordInterceptData *  data
) {
    /*
        (This code is executed by (non-initial) thread.)
        Xlib is a C library, it does not work with exception. If an exception leaked from this code,
        Xlib leaves an internal mutex locked, and program will hang. So we have to catch all the
        exceptions here.
    */
    context_t * context = reinterpret_cast< context_t * >( closure );
    // cppcheck-suppress shadowFunction
    THIS( context );
    TRACE();
    auto what = CATCH_ALL(
        intercept_data_t intercept_data( data );
        switch ( data->category ) {
            case XRecordStartOfData: {
                // XServer sends such record at the very beginning.
                context->_state = enabled;
            } break;
            case XRecordEndOfData: {
                // XServer sends such record at the very end.
                context->_state = disabled;
            } break;
            case XRecordFromServer: {
                context->_on_intercept( data );
            } break;
        }; // switch
    );
    if ( not what.empty() ) {
        context->_thread._what = what;
        CATCH_ALL( posix::signal::kill( getpid(), SIGTERM ) );
    };
}; // interceptor

context_t::thread_t::thread_t(
    display_t & display
):
    parent_t( "xctx" ),
    _display( display )
{
}; // ctor

void
context_t::thread_t::body(
) {
    TRACE();
    _display.sync();
}; // body

}; // namespace record

// =================================================================================================
// test_t
// =================================================================================================

test_t::test_t(
    display_t & display
):
    OBJECT_T(),
    _display( display )
 {
    int event_base = 0;     // Man page says event_base and error_base are not useful.
    int error_base = 0;
    int major = 0;
    int minor = 0;
    auto ok =
        XTestQueryExtension( _display, & event_base, & error_base, & major, & minor );
    if ( not ok ) {
        ERR( "XTest extension is not supported." );
    };
    DBG( "XTest extension version " << major << "." << minor << "." );
};

void
test_t::fake_key_event(
    unsigned int    key,
    bool            is_press,
    unsigned long   delay
) {
    DBG( "Emitting fake key " << key << " " << ( is_press ? "press" : "release" ) << " event…" );
    auto ok = XTestFakeKeyEvent( _display, key, is_press, delay );
    if ( not ok ) {
        ERR( "Emitting fake key event failed." );
    };
};

void
test_t::fake_button_event(
    unsigned int    button,
    bool            is_press,
    unsigned long   delay
) {
    DBG(
        "Emitting fake button " << button << " " << ( is_press ? "press" : "release" ) << " event…"
    );
    auto ok = XTestFakeButtonEvent( _display, button, is_press, delay );
    if ( not ok ) {
        ERR( "Emitting fake button event failed." );
    };
};

// -------------------------------------------------------------------------------------------------
// kb
// -------------------------------------------------------------------------------------------------

/*
    http://www.x.org/releases/X11R7.7/doc/libX11/XKB/xkblib.html
*/

kb_t::desc_t::desc_t(
    kb_t &  kb,
    uint_t  which
):
    OBJECT_T(),
    _kb( kb ),
    _rep( XkbGetKeyboard( _kb.display(), which, XkbUseCoreKbd ) )
{
    if ( not _rep ) {
        ERR( "Getting keyboard description failed." );
    }; // if
}; // ctor

kb_t::desc_t::~desc_t(
) {
    if ( _rep ) {
        XkbFreeKeyboard( _rep, 0, True );
    }; // if
}; // dtor

key_t::range_t
kb_t::desc_t::key_range(
) const {
    auto min = _rep->min_key_code;
    auto max = _rep->max_key_code;
    STATIC_ASSERT( sizeof( key_t::range_t::min ) >= sizeof( min ) );
    STATIC_ASSERT( sizeof( key_t::range_t::max ) >= sizeof( max ) );
    STATIC_ASSERT( key_t::offset == key_t::min );
    auto range = key_t::range_t( min, max );
    DBG( "Xkb reported key code range: " << range << "." );
    if ( not ( x::key_t::offset <= min and key_t::offset < max and min <= max ) ) {
        ERR( "Xkb reported invalid key code range " << range << "." );
    };
    if ( min < key_t::offset ) {
        range = key_t::range_t( min, max );
    };
    return range;
}; // key_range_t

strings_t
kb_t::desc_t::key_names(
) const {
    auto const min = _rep->min_key_code;
    auto const max = _rep->max_key_code;
    strings_t names( max + 1 );
    // Do not use `auto code = min`! `code` will be `char` and `code <= max` will be always true.
    for ( size_t code = min; code <= max; ++ code ) {
        STATIC_ASSERT( sizeof( code ) > sizeof( max ) );
        /*
            Key name is a fixed-length string padded with zeros. However, if name has maximal
            length, there is *no* trailing zero, so trivial `string_t( name )` does not work,
            `string_t( name, XkbKeyNameLength )` does not wor too. Let's trim trailing zeros.
        */
        char const * name = _rep->names->keys[ code ].name;
        size_t len = XkbKeyNameLength;
        while ( len > 0 and name[ len - 1 ] == 0 ) {
            -- len;
        }; // while
        names[ code ] = string_t( name, len );
    }; // for code
    return names;
}; // key_names

strings_t
kb_t::desc_t::group_names(
) const {
    strings_t group_names( layout_max );
    for ( size_t i = 0, end = layout_max; i < end; ++ i ) {
        if ( _rep->names->groups[ i ] != None ) {
            group_names[ i ] = atom_t( _kb.display(), _rep->names->groups[ i ] ).name();
        }; // if
    }; // for i
    return group_names;
}; // group_names

kb_t::kb_t(
    display_t & display
):
    OBJECT_T(),
    _display( display )
{
    {
        int major = XkbMajorVersion;
        int minor = XkbMinorVersion;
        Bool ok = XkbLibraryVersion( & major, & minor );
        if ( not ok ) {
            ERR(
                "X keyboard extension: "
                    << "Incompatible client library version " << major << "." << minor << "; "
                    << "required version " << XkbMajorVersion << "." << XkbMinorVersion << "."
            );
        }; // if
        DBG( "X keyboard extension: client library version " << major << "." << minor << "." );
    }
    {
        int major = XkbMajorVersion;
        int minor = XkbMinorVersion;
        int dummy1, dummy2, dummy3;
        Bool ok = XkbQueryExtension( _display, & dummy1, & dummy2, & dummy3, & major, & minor );
        if ( not ok ) {
            ERR(
                "X keyboard extension: "
                "Incompatible server extension " << major << "." << minor << "; "
                    << "required version " << XkbMajorVersion << "." << XkbMinorVersion << "."
            );
        }; // if
        DBG( "X keyboard extension: server extension version " << major << "." << minor << "." );
    }
}; // ctor

display_t &
kb_t::display(
) {
    return _display;
}; // display

kb_t::autorepeat_rate_t
kb_t::autorepeat_rate(
) {
    autorepeat_rate_t rate;
    Bool ok = XkbGetAutoRepeatRate( _display, XkbUseCoreKbd, & rate.timeout, & rate.interval );
    if ( not ok ) {
        ERR( "Getting autorepeat rate failed." );
    }; // if
    DBG( "Autorepeat rate: timeout = " << rate.timeout << ", interval = " << rate.interval << "." );
    return rate;
}; // autorepeat_rate

void
kb_t::lock_group(
    uint_t  group
) {
    TRACE();
    Bool ok = XkbLockGroup( _display, XkbUseCoreKbd, group );
    if ( not ok ) {
        ERR( "Locking group " << ( group + 1 ) << " failed." );
    }; // if
    DBG( "Group " << ( group + 1 ) << " locked." );
}; // lock_group

void
kb_t::bell(
) {
    Bool ok = XkbBell( _display, None, /* volume in range [-100…100] */ 100, /* name */ None );
    if ( not ok ) {
        WRN( "Bell failed." );
    }; // if
}; // bell

kb_t::desc_p
kb_t::desc(
    uint_t  which
) {
    return desc_p( new desc_t( * this, which ) );
}; // desc

}; // namespace x
}; // namespace tapper

// end of file //
