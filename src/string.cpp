/*
    ---------------------------------------------------------------------- copyright and license ---

    File: src/string.cpp

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
    String manipulating utilities implementation.
**/

#include "string.hpp"

#include <algorithm>
#include <locale>
#include <limits>
#include <system_error>

#if WITH_GLIB
    #include <glibmm/stringutils.h>
#endif // WITH_GLIB

#include "test.hpp"

namespace tapper {

#if ! WITH_GLIB

    static
    string_t
    oct(
        char c
    ) {
        using uchar_t = unsigned char;
        static char digits[] = "01234567";
        char oct[ 5 ] = {
            '\\',
            digits[ ( uchar_t( c ) >> 6 ) & 0x7 ],
            digits[ ( uchar_t( c ) >> 3 ) & 0x7 ],
            digits[ ( uchar_t( c ) >> 0 ) & 0x7 ],
            0
        };
        return oct;
    }; // oct

    TEST(
        ASSERT_EQ( oct(   0 ), "\\000" );
        ASSERT_EQ( oct(   1 ), "\\001" );
        ASSERT_EQ( oct(   7 ), "\\007" );
        ASSERT_EQ( oct(   8 ), "\\010" );
        ASSERT_EQ( oct(  73 ), "\\111" );
        ASSERT_EQ( oct( 255 ), "\\377" );
    );

#endif // ! WITH_GLIB

//  ------------------------------------------------------------------------------------------------
//  chop
//  ------------------------------------------------------------------------------------------------

string_t
chop(
    string_t const & str,   ///< String to chop suffix from.
    string_t const & sfx    ///< Suffix to chop.
) {
    if ( right( str, sfx.size() ) == sfx ) {
        return left( str, str.size() - sfx.size() );
    } else {
        return str;
    }; // if
}; // chop

TEST(
    ASSERT_EQ( chop( "", "" ), "" );

    ASSERT_EQ( chop( "abc", "" ), "abc" );
    ASSERT_EQ( chop( "", "z" ), "" );
    ASSERT_EQ( chop( "abc", "z" ), "abc" );
    ASSERT_EQ( chop( "abcz", "z" ), "abc" );
    ASSERT_EQ( chop( "abczz", "z" ), "abcz" ); // chops only once.

    ASSERT_EQ( chop( "эюя", "" ), "эюя" );
    ASSERT_EQ( chop( "", "ё"  ), "" );
    ASSERT_EQ( chop( "эюя", "ё" ), "эюя" );
    ASSERT_EQ( chop( "эюяё", "ё" ), "эюя" );
    ASSERT_EQ( chop( "эюяёё", "ё" ), "эюяё" ); // chops only once.
);

//  ------------------------------------------------------------------------------------------------
//  concat
//  ------------------------------------------------------------------------------------------------

strings_t
concat(
    string_t const &  lhs,
    strings_t const & rhs
) {
    return concat( strings_t( 1, lhs ), rhs );
};

TEST(
    ASSERT_EQ( join( ", ", concat( string_t( "a" ), strings_t( {} ) ) ), "a" );
    ASSERT_EQ( join( ", ", concat( string_t( "a" ), strings_t( { "b", "c" } ) ) ), "a, b, c" );
);

strings_t
concat(
    strings_t const & lhs,
    strings_t const & rhs
) {
    strings_t result = lhs;
    std::copy( rhs.begin(), rhs.end(), std::back_inserter( result ) );
    return result;
};

TEST(
    ASSERT_EQ( join( ", ", concat( strings_t( {} ), strings_t( {} ) ) ), "" );
    ASSERT_EQ( join( ", ", concat( strings_t( { "a" } ), strings_t( { "b" } ) ) ), "a, b" );
);

//  ------------------------------------------------------------------------------------------------
//  errstr
//  ------------------------------------------------------------------------------------------------

string_t
errstr(
    int                 err,
    string_t const &    msg
) {
    return
        string_t( std::system_error( err, std::generic_category(), chop( msg, "." ) ).what() )
            + ".";
};

//  ------------------------------------------------------------------------------------------------
//  find
//  ------------------------------------------------------------------------------------------------

/**
    Searches a string vector for a string.

    @return Index of the found string, or `npos` if the string is not found.
**/
size_t
find(
    string_t const &  needle,   ///< String to search for.
    strings_t const & haystack  ///< Strings to search in.
) {
    auto it = std::find( haystack.begin(), haystack.end(), needle );
    return it == haystack.end() ? npos : it - haystack.begin();
}; // find

//  ------------------------------------------------------------------------------------------------
//  front
//  ------------------------------------------------------------------------------------------------

char_t
front(
    string_t const & string
) {
    return string.empty() ? 0 : string[ 0 ];
};

TEST(
    ASSERT_EQ( front( ""      ), char_t(   0 ) );
    ASSERT_EQ( front( "12345" ), char_t( '1' ) );
);

//  ------------------------------------------------------------------------------------------------
//  has_prefix
//  ------------------------------------------------------------------------------------------------

bool
has_prefix(
    string_t const & string,
    string_t const & prefix
) {
    return string.substr( 0, prefix.size() ) == prefix;
};

TEST(
    ASSERT( has_prefix( "@1", "@" ) );
    ASSERT( has_prefix( "+-100", "+-" ) );
    ASSERT( has_prefix( "⇵100", "⇵" ) );
    // Corner case:
    ASSERT( has_prefix( "⇵100", "" ) );
    ASSERT( has_prefix( "", "" ) );
);

//  ------------------------------------------------------------------------------------------------
//  is_one_of
//  ------------------------------------------------------------------------------------------------

bool
is_one_of(
    char_t           ch,
    string_t const & string
) {
    return string.find_first_of( ch ) != npos;
};

TEST(
    ASSERT(     is_one_of( 'a', "abc" ) );
    ASSERT( not is_one_of( 'z', "abc" ) );
);

//  ------------------------------------------------------------------------------------------------
//  join
//  ------------------------------------------------------------------------------------------------

/**
    Perl-like `join`.
**/
string_t
join(
    string_t const &  joiner,
    strings_t const & strings
) {
    string_t r;
    for ( size_t i = 0, end = strings.size(); i < end; ++ i ) {
        r += ( i == 0 ? "" : joiner );
        r += strings[ i ];
    };
    return r;
};

TEST(
    ASSERT_EQ( join( ", ", strings_t( { "a", "b", "c" } ) ), "a, b, c" );
    ASSERT_EQ( join( ", ", strings_t( { "a", "b" } ) ), "a, b" );
    ASSERT_EQ( join( ", ", strings_t( { "a" } ) ), "a" );
    ASSERT_EQ( join( ", ", strings_t( { } ) ), "" );

    ASSERT_EQ( join( "", strings_t( { "a", "b", "c" } ) ), "abc" );
    ASSERT_EQ( join( "", strings_t( { "a", "b" } ) ), "ab" );
    ASSERT_EQ( join( "", strings_t( { "a" } ) ), "a" );
    ASSERT_EQ( join( "", strings_t( { } ) ), "" );

    ASSERT_EQ( join( ", ", strings_t( { "", "", "" } ) ), ", , " );
    ASSERT_EQ( join( "", strings_t( { "" } ) ), "" );

    ASSERT_EQ( join( ", ", strings_t( { "э", "ю", "я" } ) ), "э, ю, я" );
    ASSERT_EQ( join( ", ", strings_t( { "э", "ю" } ) ), "э, ю" );
    ASSERT_EQ( join( ", ", strings_t( { "э" } ) ), "э" );
    ASSERT_EQ( join( ", ", strings_t( { } ) ), "" );
);

//  ------------------------------------------------------------------------------------------------
//  lc
//  ------------------------------------------------------------------------------------------------

/**
    Returns an uppercased version of string. If Gnome support is disabled, the function works only
    with ASCII characters.
**/
string_t
lc(
    string_t const & str
) {
    #if WITH_GLIB
        return str.lowercase();
    #else
        string_t res;
        res.reserve( str.size() );
        for ( size_t i = 0, end = str.size(); i < end; ++ i ) {
            res += tolower( str[ i ] );
        }; // for
        return res;
    #endif // WITH_GLIB
}; // lc

TEST(
    ASSERT_EQ( lc( "" ), "" );
    ASSERT_EQ( lc( "1234567890-+=" ), "1234567890-+=" );

    ASSERT_EQ( lc( "abc" ), "abc" );
    ASSERT_EQ( lc( "ABC" ), "abc" );
    if ( WITH_GLIB ) {
        ASSERT_EQ( lc( "эюя" ), "эюя" );
        ASSERT_EQ( lc( "ЭЮЯ" ), "эюя" );
    }; // WITH_GLIB
);

//  ------------------------------------------------------------------------------------------------
//  left
//  ------------------------------------------------------------------------------------------------

/**
    Extracts specified number of characters from the beginning of a string. If the string is
    shorter than requested number of characters, all characters of the string are returned.

    @return String containing extracted characters.
**/
string_t
left(
    string_t const & str,   ///< String to extract characters from.
    size_t           len    ///< Number of characters to extract.
) {
    return str.substr( 0, len );
}; // left

TEST(
    ASSERT_EQ( left( "", 4 ), "" );

    ASSERT_EQ( left( "abc", 0 ), "" );
    ASSERT_EQ( left( "abc", 1 ), "a" );
    ASSERT_EQ( left( "abc", 2 ), "ab" );
    ASSERT_EQ( left( "abc", 3 ), "abc" );
    ASSERT_EQ( left( "abc", 4 ), "abc" );

    if ( WITH_GLIB ) {
        ASSERT_EQ( left( "эюя", 0 ), "" );
        ASSERT_EQ( left( "эюя", 1 ), "э" );
        ASSERT_EQ( left( "эюя", 2 ), "эю" );
        ASSERT_EQ( left( "эюя", 3 ), "эюя" );
        ASSERT_EQ( left( "эюя", 4 ), "эюя" );
    }; // WITH_GLIB
);

//  ------------------------------------------------------------------------------------------------
//  like_cardinal
//  ------------------------------------------------------------------------------------------------

/**
    Returns `true` if a string looks like an cardinal number (unsigned integer). More precisely,
    returns `true` if the string comprises only digits (no leading or trailing spaces are allowed),
    and `false` otherwise.
**/
bool
like_cardinal(
    string_t const & string,
    size_t           pos
) {
    return pos < string.size() and string.find_first_not_of( "0123456789", pos ) == npos;
}; // like_cardinal

TEST(
    ASSERT( not like_cardinal( ""     ) );
    ASSERT( not like_cardinal( "abcd" ) );
    ASSERT( not like_cardinal( "   1" ) );
    ASSERT( not like_cardinal( "1   " ) );
    ASSERT( not like_cardinal( "+100" ) );
    ASSERT( not like_cardinal( "-100" ) );

    ASSERT( like_cardinal( "0" ) );
    ASSERT( like_cardinal( "1" ) );
    ASSERT( like_cardinal( "1234567890" ) );
    ASSERT( like_cardinal( "123456789012345678901234567890123456789012345678901234567890" ) );

    ASSERT( not like_cardinal( "",     3 ) );
    ASSERT( not like_cardinal( "abcd", 3 ) );
    ASSERT(     like_cardinal( "   1", 3 ) );
    ASSERT( not like_cardinal( "1   ", 2 ) );
    ASSERT(     like_cardinal( "+100", 1 ) );
    ASSERT(     like_cardinal( "-100", 1 ) );
);

//  ------------------------------------------------------------------------------------------------
//  like_integer
//  ------------------------------------------------------------------------------------------------

/**
    Returns `true` if a string looks like an integer number. More precisely, returns `true` if the
    string comprises only digits (no leading or trailing spaces are allowed), and `false`
    otherwise.
**/
bool
like_integer(
    string_t const & string
) {
    size_t begin = ( not string.empty() and ( string[ 0 ] == '+' or string[ 0 ] == '-' ) ) ? 1 : 0;
    return like_cardinal( string, begin );
}; // like_integer

TEST(
    ASSERT( like_integer( "0" ) );
    ASSERT( like_integer( "1" ) );
    ASSERT( like_integer( "+12" ) );
    ASSERT( like_integer( "-34" ) );
    ASSERT( like_integer( "1111111111111111111111111111111111111111111111111111111111111111111" ) );
    ASSERT( like_integer( "+111111111111111111111111111111111111111111111111111111111111111111" ) );
    ASSERT( like_integer( "-111111111111111111111111111111111111111111111111111111111111111111" ) );
    ASSERT( not like_integer( "" ) );
    ASSERT( not like_integer( "a" ) );
    ASSERT( not like_integer( "я" ) );
    ASSERT( not like_integer( "1.1" ) );
    ASSERT( not like_integer( "+1.2" ) );
    ASSERT( not like_integer( "-3.4" ) );
    ASSERT( not like_integer( "1a" ) );
    ASSERT( not like_integer( "1я" ) );
    ASSERT( not like_integer( "a1" ) );
    ASSERT( not like_integer( "я1" ) );
    ASSERT( not like_integer( "1 " ) );
    ASSERT( not like_integer( " 1" ) );
);

//  ------------------------------------------------------------------------------------------------
//  match
//  ------------------------------------------------------------------------------------------------

std::cmatch
match(
    string_t const &  string,
    string_t const &  rex
) {
    std::cmatch m;
    std::regex_match(
        string.c_str(),
        m,
        std::regex( rex.c_str(), std::regex::ECMAScript | std::regex::icase )
    );
    return m;
};

//  ------------------------------------------------------------------------------------------------
//  pad_l
//  ------------------------------------------------------------------------------------------------

string_t
pad_l(
    string_t const & str,
    size_t           len,
    char             pad
) {
    if ( str.size() < len ) {
        return string_t( len - str.size(), pad ) + str;
    } else {
        return str;
    };
};

TEST(

    ASSERT_EQ( pad_l( "x",    3 ), "  x"  );
    ASSERT_EQ( pad_l( "xx",   3 ), " xx"  );
    ASSERT_EQ( pad_l( "xxx",  3 ), "xxx"  );
    ASSERT_EQ( pad_l( "xxxx", 3 ), "xxxx" );

    ASSERT_EQ( pad_l( "x",    3, '-' ), "--x"  );
    ASSERT_EQ( pad_l( "xx",   3, '-' ), "-xx"  );
    ASSERT_EQ( pad_l( "xxx",  3, '-' ), "xxx"  );
    ASSERT_EQ( pad_l( "xxxx", 3, '-' ), "xxxx" );

);

//  ------------------------------------------------------------------------------------------------
//  pad_r
//  ------------------------------------------------------------------------------------------------

string_t
pad_r(
    string_t const & str,
    size_t           len,
    char             pad
) {
    if ( str.size() < len ) {
        return str + string_t( len - str.size(), pad );
    } else {
        return str;
    };
};

TEST(

    ASSERT_EQ( pad_r( "x",    3 ), "x  "  );
    ASSERT_EQ( pad_r( "xx",   3 ), "xx "  );
    ASSERT_EQ( pad_r( "xxx",  3 ), "xxx"  );
    ASSERT_EQ( pad_r( "xxxx", 3 ), "xxxx" );

    ASSERT_EQ( pad_r( "x",    3, '-' ), "x--"  );
    ASSERT_EQ( pad_r( "xx",   3, '-' ), "xx-"  );
    ASSERT_EQ( pad_r( "xxx",  3, '-' ), "xxx"  );
    ASSERT_EQ( pad_r( "xxxx", 3, '-' ), "xxxx" );

);

//  ------------------------------------------------------------------------------------------------
//  q
//  ------------------------------------------------------------------------------------------------

/**
    Returns quoted string (Unicode single quotation mark used, U+2018 ‘ and U+2019 ’). Control
    characters, if any, are replaced with appropriate Unicode symbols (U+2400 ␀, … U+2421 ␡, which
    are printable characters, *not* control ones). Unicode control symbols and single quotes are
    escaped by backslash. Backslash is escaped, too. But ASCII quotes U+0022 " does not.
**/
string_t
q(
    string_t const & string
) {
    static char const * const control[] = {
            "␀", "␁", "␂", "␃", "␄", "␅", "␆", "␇", "␈", "␉", "␊", "␋", "␌", "␍", "␎", "␏",
            "␐", "␑", "␒", "␓", "␔", "␕", "␖", "␗", "␘", "␙", "␚", "␛", "␜", "␝", "␞", "␟"
    };
    string_t    result;
    result.reserve( string.size() + 16 );
    result += "‘";
    for ( auto c: string ) {
        if ( 0 <= c and c < ' ' ) {
            result += control[ int( c ) ];
        } else if ( c == '\x7F' ) {
            result += "␡";
        } else if ( c == '\\' ) {
            result += '\\';
            result += c;
        } else if ( c == 0x2018 or c == 0x2019 or ( 0x2400 <= c and c <= 0x2421 ) ) {
            //           ‘              ’           ␀                    ␡
            // Seems Unicode character literals are not very portable, let's use hex values instead.
            result += '\\';
            result += c;
        } else {
            result += c;
        };
    };
    result += "’";
    return  result;
};

TEST(
    ASSERT_EQ( q( R"()"                 ), R"(‘’)"         );
    ASSERT_EQ( q( R"('")"               ), R"(‘'"’)"       );
    ASSERT_EQ( q(  "\r\n\\\x7F"         ), R"(‘␍␊\\␡’)"    );
    ASSERT_EQ( q( R"(йцукенгш)"         ), R"(‘йцукенгш’)" );
    ASSERT_EQ( q( string_t( 3, '\x00' ) ), R"(‘␀␀␀’)"      );
    ASSERT_EQ( q(  "␍␊␡"                ), R"(‘\␍\␊\␡’)"   );
    ASSERT_EQ( q(  "‘’"                 ), R"(‘\‘\’’)"     );
);

//  ------------------------------------------------------------------------------------------------
//  right
//  ------------------------------------------------------------------------------------------------

/**
    Extracts specified number of characters from the end of a string. If the string is shorter than
    requested number of characters, all characters of the string are returned.

    @return String containing extracted characters.
**/
string_t
right(
    string_t const & str,   ///< String to extract characters from.
    size_t           len    ///< Number of characters to extract.
) {
    return str.substr( str.size() >= len ? str.size() - len : 0, len );
}; // right

TEST(
    ASSERT_EQ( right( "", 4 ), "" );

    ASSERT_EQ( right( "abc", 0 ), "" );
    ASSERT_EQ( right( "abc", 1 ), "c" );
    ASSERT_EQ( right( "abc", 2 ), "bc" );
    ASSERT_EQ( right( "abc", 3 ), "abc" );
    ASSERT_EQ( right( "abc", 4 ), "abc" );

    if ( WITH_GLIB ) {
        ASSERT_EQ( right( "эюя", 0 ), "" );
        ASSERT_EQ( right( "эюя", 1 ), "я" );
        ASSERT_EQ( right( "эюя", 2 ), "юя" );
        ASSERT_EQ( right( "эюя", 3 ), "эюя" );
        ASSERT_EQ( right( "эюя", 4 ), "эюя" );
    }; // WITH_GLIB
);

//  ------------------------------------------------------------------------------------------------
//  split
//  ------------------------------------------------------------------------------------------------

/**
    Perl-like `split`.
**/
strings_t
split(
    char              chr,  ///< Delimiter.
    string_t const &  str,  ///< String to split.
    size_t            limit ///< Maximal number of strings to return.
) {
    strings_t res;
    size_t size = str.size();
    if ( not limit ) {
        limit = std::numeric_limits< size_t >::max();
    };
    size_t p = 0;
    while ( limit > 1 and p < size ) {
        // cppcheck-suppress shadowFunction
        auto q = str.find( chr, p );
        if ( q == npos ) {
            q = size;
        };
        res.push_back( str.substr( p, q - p ) );
        p = q + 1;
        limit -= 1;
    }; // while
    if ( p <= size ) {
        res.push_back( str.substr( p, size - p ) );
    }; // if
    return res;
}; // split

TEST(
    ASSERT_EQ( split( '=', "abc", 4 ), strings_t( { "abc" } ) );
    ASSERT_EQ( split( '=', "",    4 ), strings_t( { ""    } ) );
    ASSERT_EQ( split( '=', "",    0 ), strings_t( { ""    } ) );
    ASSERT_EQ( split( '=', ""       ), strings_t( { ""    } ) );

    ASSERT_EQ( split( '=', "a=b=c"    ), strings_t( { "a", "b", "c" } ) );
    ASSERT_EQ( split( '=', "a=b=c", 0 ), strings_t( { "a", "b", "c" } ) );
    ASSERT_EQ( split( '=', "a=b=c", 1 ), strings_t( { "a=b=c"       } ) );
    ASSERT_EQ( split( '=', "a=b=c", 2 ), strings_t( { "a", "b=c"    } ) );
    ASSERT_EQ( split( '=', "a=b=c", 3 ), strings_t( { "a", "b", "c" } ) );
    ASSERT_EQ( split( '=', "a=b=c", 4 ), strings_t( { "a", "b", "c" } ) );

    ASSERT_EQ( split( '=', "=="    ), strings_t( { "", "", "" } ) );
    ASSERT_EQ( split( '=', "==", 0 ), strings_t( { "", "", "" } ) );
    ASSERT_EQ( split( '=', "==", 1 ), strings_t( { "=="       } ) );
    ASSERT_EQ( split( '=', "==", 2 ), strings_t( { "", "="    } ) );
    ASSERT_EQ( split( '=', "==", 3 ), strings_t( { "", "", "" } ) );
    ASSERT_EQ( split( '=', "==", 4 ), strings_t( { "", "", "" } ) );

    ASSERT_EQ( split( '=', "эюя", 4 ), strings_t( { "эюя" } ) );

    ASSERT_EQ( split( '=', "э=ю=я"    ), strings_t( { "э", "ю", "я" } ) );
    ASSERT_EQ( split( '=', "э=ю=я", 0 ), strings_t( { "э", "ю", "я" } ) );
    ASSERT_EQ( split( '=', "э=ю=я", 1 ), strings_t( { "э=ю=я"       } ) );
    ASSERT_EQ( split( '=', "э=ю=я", 2 ), strings_t( { "э", "ю=я"    } ) );
    ASSERT_EQ( split( '=', "э=ю=я", 3 ), strings_t( { "э", "ю", "я" } ) );
    ASSERT_EQ( split( '=', "э=ю=я", 4 ), strings_t( { "э", "ю", "я" } ) );
);

//  ------------------------------------------------------------------------------------------------
//  trim
//  ------------------------------------------------------------------------------------------------

string_t
trim(
    string_t const & str
) {
    return trim_l( trim_r( str ) );
};

//  ------------------------------------------------------------------------------------------------
//  trim_l
//  ------------------------------------------------------------------------------------------------

string_t
trim_l(
    string_t const & str
) {
    auto p = str.find_first_not_of( " " );
    return p == npos ? "" : str.substr( p );
};

TEST(
    ASSERT_EQ( trim_l( ""       ), "" );
    ASSERT_EQ( trim_l( " "      ), "" );
    ASSERT_EQ( trim_l( "  "     ), "" );

    ASSERT_EQ( trim_l( "word "   ), "word " );
    ASSERT_EQ( trim_l( " word "  ), "word " );
    ASSERT_EQ( trim_l( "  word " ), "word " );
);

//  ------------------------------------------------------------------------------------------------
//  trim_r
//  ------------------------------------------------------------------------------------------------

string_t
trim_r(
    string_t const & str
) {
    auto p = str.find_last_not_of( " " );
    return p == npos ? "" : str.substr( 0, p + 1 );
};

TEST(
    ASSERT_EQ( trim_r( ""       ), "" );
    ASSERT_EQ( trim_r( " "      ), "" );
    ASSERT_EQ( trim_r( "  "     ), "" );

    ASSERT_EQ( trim_r( " word"   ), " word" );
    ASSERT_EQ( trim_r( " word "  ), " word" );
    ASSERT_EQ( trim_r( " word  " ), " word" );
);

//  ------------------------------------------------------------------------------------------------
//  uc
//  ------------------------------------------------------------------------------------------------

/**
    Returns an uppercased version of string. If Gnome support is disabled, the function works only
    with ASCII characters.
**/
string_t
uc(
    string_t const & str
) {
    #if WITH_GLIB
        return str.uppercase();
    #else
        string_t res;
        res.reserve( str.size() );
        for ( size_t i = 0, end = str.size(); i < end; ++ i ) {
            res += toupper( str[ i ] );
        }; // for
        return res;
    #endif // WITH_GLIB
}; // uc

TEST(
    ASSERT_EQ( uc( "" ), "" );
    ASSERT_EQ( uc( "1234567890-+=" ), "1234567890-+=" );

    ASSERT_EQ( uc( "abc" ), "ABC" );
    ASSERT_EQ( uc( "ABC" ), "ABC" );
    if ( WITH_GLIB ) {
        ASSERT_EQ( uc( "эюя" ), "ЭЮЯ" );
        ASSERT_EQ( uc( "ЭЮЯ" ), "ЭЮЯ" );
    }; // WITH_GLIB
);

}; // namespace tapper

// end of file //
