#!/usr/bin/perl

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/news-to-releases.pl
#
#   Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
#
#   This file is part of Tapper.
#
#   Tapper is free software: you can redistribute it and/or modify it under the terms of the GNU
#   General Public License as published by the Free Software Foundation, either version 3 of the
#   License, or (at your option) any later version.
#
#   Tapper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
#   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along with Tapper.  If not,
#   see <http://www.gnu.org/licenses/>.
#
#   SPDX-License-Identifier: GPL-3.0-or-later
#
#   ---------------------------------------------------------------------- copyright and license ---

use strict;
use warnings;
use utf8;
use open IO => ':utf8';

use Getopt::Long;
use HTML::Entities ();
use JSON ();
use Path::Tiny ();
use Pod::Usage ();

{
    no warnings qw( redefine );
    *Path::Tiny::Error::throw = sub {
        my ( $class, $op, $file, $err ) = @_;
        chomp( my $trace = $Carp::Verbose ? Carp::shortmess : '' );
        my $msg = "Error $op on '$file': $err$trace\n";
        die bless { op => $op, file => $file, err => $err, msg => $msg }, $class;
    };
};

=encoding UTF-8

=cut

#   ------------------------------------------------------------------------------------------------

sub ver_cmp($$) {
    my ( $a, $b ) = @_;
    my @a = split( /\./, $a );
    my @b = split( /\./, $b );
    my $i = 0;
    while ( $i < @a and $i < @b and $a[ $i ] == $b[ $i ] ) {
        ++ $i;
    };
    if ( $i >= @a and $i >= @b ) {
        return 0;
    };
    if ( $i < @a and $i >= @b ) {
        return 1;
    };
    if ( $i >= @a and $i < @b ) {
        return -1;
    };
    return $a[ $i ] <=> $b[ $i ];
};

#   ------------------------------------------------------------------------------------------------

package News {

=begin comment

    Parameters:

    *   input => $string (optional), name of JSON file to read.
    *   output => $string (optional), name of XML file to write.

    Returns:

    *   $self.

=end comment
=cut

sub new {
    my ( $class, %args ) = @_;
    my $self = {
        %args
    };
    return bless( $self, $class );
};

=begin comment

    Parameters:

    *   $input — Name of JSON file to read. If undefined, `input` ctor parameter will be used.
        If undefined, STDIN will be read.

    Returns:

    *   $self.

=end comment
=cut

sub load_json {
    my ( $self, $input ) = @_;
    $input //= $self->{ input };
    my $text = do {
        if ( defined( $input ) ) {
            $self->{ file } = $input;
            Path::Tiny::path( $input )->slurp_utf8();
        } else {
            $self->{ file } = '(*stdin*)';
            local $/ = undef;
            STDIN->getline();
        };
    };
    my $n = News::JSON->new( $text );
    if ( $self->{ news } ) {
        $self->_merge( $n->{ news } );
    } else {
        $self->{ news } = $n->{ news };
    };
    delete $self->{ file };
    return $self;
};

sub _merge {
    my ( $self, $news ) = @_;
    for my $n ( $news->{ releases }->@* ) {
        my $version = $n->{ header }->{ version };
        my $date    = $n->{ header }->{ date    };
        my @r =
            grep(
                { $_->{ header }->{ version } eq $version and $_->{ header }->{ date } eq $date }
                $self->{ news }->{ releases }->@*
            );
        @r > 0 or die "Can't merge: No ‘$version @ $date’ release found.\n";
        @r == 1 or die "oops";
        push( $r[ 0 ]->{ description }->@*, $n->{ description }->@* );
    };
};

=begin comment

    Parameters:

    *   $output — Name of XML file to write. If undefined, the result will be printed to STDOUT.

    Returns:

    *   $self.

=end comment
=cut

sub save_xml {
    my ( $self, $output ) = @_;
    $output //= $self->{ output };
    my $xml = News::XML->new(
        limit   => $self->{ limit },
        news    => $self->{ news },
        url     => $self->{ url },
    )->xml();
    if ( defined( $output ) ) {
        Path::Tiny::path( $output )->spew_utf8( $xml );
    } else {
        STDOUT->print( $xml );
    };
    return $self;
};

};

#   ------------------------------------------------------------------------------------------------
#   News::JSON — convert JSON News to internal representation.
#   ------------------------------------------------------------------------------------------------

package News::JSON {

sub new {
    my ( $class, $text ) = @_;
    my $self = bless( {}, $class );
    if ( defined( $text ) ) {
        $self->parse( $text );
    };
    return $self;
};

sub parse {
    my ( $self, $text ) = @_;
    my $json = JSON->new->decode( $text );
        #   $json = {
        #       'pandoc-api-version' => [ 1, 22 ],
        #       'meta'               => { ... },
        #       'blocks'             => [ ... ],
        #   };
    $self->{ lang } = $self->_lang( $json->{ meta } );
    if ( defined( $self->{ lang } ) and $self->{ lang } eq 'en' ) {
        delete $self->{ lang };
    };
    $self->{ news } = $self->_news( $json->{ blocks } ),
    return $self;
};

sub _e {
    my ( $self, $msg, $json ) = @_;
    Carp::confess(
        ( $json ? "$0: error: JSON reading stopped at: " . DDP::np( $json ) . "\n" : '' ) .
        "$0: error: " . $msg . "\n" .
        "Stopped"
    );
};

sub _lang {
    my ( $self, $json ) = @_;
    if ( not $json->{ lang } ) {
        return undef;
    };
    $json->{ lang }->{ t } eq 'MetaInlines' or die;
    my $lang = $self->_spans( $json->{ lang }->{ c } );
    @$lang == 1
        and $lang->[ 0 ]->{ type } eq 'plain'
        or die "TODO";
    $lang = $lang->[ 0 ]->{ text };
    $lang =~ m{\A[a-z]{2}\z}
        or die "Invalid language code: ‘$lang’.\n";
    return $lang;
};

sub _news {
    my ( $self, $json ) = @_;
        #   $json = [
        #       { 't' => $type0, 'c' => $content0 },
        #       { 't' => $type1, 'c' => $content1 },
        #       ...
        #   ];
    # Skip comments:
    while ( @$json and $json->[ 0 ]->{ t } eq 'RawBlock' ) {
        $self->_comment( $json );
    };
    # Skip the first level header:
        #   {
        #       't' => 'Header',
        #       'c' => [
        #           $level,     # e. g. 1, 2, 3...
        #           [ ... ],    # anchor
        #           [ ... ],    # text
        #       ],
        #   };
    @$json and $json->[ 0 ]->{ t } eq 'Header' and $json->[ 0 ]->{ c }->[ 0 ] == 1
        or $self->_e( "The first level heder expected", $json->[ 0 ] );
    shift( @$json );
    # Skip everything up to the next header:
    while ( @$json and $json->[ 0 ]->{ t } ne 'Header' ) {
        shift( @$json );
    };
    # Parse releases:
    my $releases = $self->_releases( $json );
    # Skip comments, if any:
    while ( @$json and $json->[ 0 ]->{ t } eq 'RawBlock' ) {
        shift( @$json );
    };
    not @$json
        or $self->_e( "Unexpected blocks found", $json->[ 0 ] );
    return {
        releases => $releases,
    };
};

sub _releases {
    my ( $self, $json ) = @_;
    my $releases = [];
    while ( @$json and $json->[ 0 ]->{ t } eq 'Header' ) {
        push( @$releases, $self->_release( $json ) );
    };
    return $releases;
};

sub _release {
    my ( $self, $json ) = @_;
    # Parse the second level header:
    my $header = $self->_header( $json );
    # Parse the section body:
    my $description = $self->_description( $json );
    return {
        header      => $header,
        description => $description,
    };
};

sub _header {
    my ( $self, $json ) = @_;
    $json->[ 0 ]->{ t } eq 'Header' and $json->[ 0 ]->{ c }->[ 0 ] == 2
        or $self->_e( "The second level header expected", $json->[ 0 ] );
    my $blocks = $json->[ 0 ]->{ c }->[ 2 ];
    $blocks->[ 0 ]->{ t } eq 'Str'
        and $blocks->[ 1 ]->{ t } eq 'Space'
        and $blocks->[ 2 ]->{ t } eq 'Str'
        and $blocks->[ 2 ]->{ c } eq '@'
        and $blocks->[ 3 ]->{ t } eq 'Space'
        and $blocks->[ 4 ]->{ t } eq 'Str'
        and @$blocks == 5
        or $self->_e( "The header text should look like ‘version @ date’", $json->[ 0 ] );
    my $version = $blocks->[ 0 ]->{ c };
    my $date    = $blocks->[ 4 ]->{ c };
    $version =~ m{\A\d+(\.\d+)+\z}
        or $self->_e( "Bad version string in the header: $version" );
    $date =~ m{\A\d\d\d\d-\d\d-\d\d\z}
        or $self->_e( "Bad date string in the header: $date" );
    if ( my $last = $self->{ _last_header } ) {
        # Check version and date ordering (released must follow in reverse chronological order):
        my @order = ( '<', '=', '>' );
        my $v = ::ver_cmp( $last->{ version }, $version );
        $v > 0
            or $self->_e( "Bad release order: ‘$last->{ version }’ $order[ $v + 1 ] ‘$version’\n" );
        my $d = $last->{ date } cmp $date;
        $d > 0
            or $self->_e( "Bad release order: ‘$last->{ date }’ $order[ $d + 1 ] ‘$date’\n" );
    };
    shift( @$json );
    my $header = {
        version => $version,
        date    => $date,
    };
    $self->{ _last_header } = $header;
    return $header;
};

sub _description {
    my ( $self, $json ) = @_;
    my $blocks = [];
    while ( @$json and $json->[ 0 ]->{ t } ne 'Header' ) {
        push( @$blocks, $self->_block( $json ) );
    };
    return $blocks;
};

sub _block {
    my ( $self, $json ) = @_;
    my $block;
    if ( $json->[ 0 ]->{ t } eq 'Para' ) {
        $block = $self->_para( $json );
    } elsif ( $json->[ 0 ]->{ t } eq 'BulletList' ) {
        $block = $self->_ulist( $json );
    } elsif ( $json->[ 0 ]->{ t } eq 'OrderedList' ) {
        $block = $self->_olist( $json );
    } elsif ( $json->[ 0 ]->{ t } eq 'RawBlock' ) {
        $block = $self->_comment( $json );
    } else {
        $self->_e( "Unsupported block type found", $json->[ 0 ] );
    };
    return $block;
};

sub _comment {
    my ( $self, $json ) = @_;
    $json->[ 0 ]->{ t } eq 'RawBlock'
        or $self->_e( "oops", $json->[ 0 ] );
    $json->[ 0 ]->{ c }->[ 0 ] eq 'html'
        and $json->[ 0 ]->{ c }->[ 1 ] =~ m{\A<!--.*-->\z}s
        or $self->_e( "HTML tags are not allowed", $json->[ 0 ] );
    my $text = $json->[ 0 ]->{ c }->[ 1 ];
    shift( @$json );
    return {
        type => 'comm',
        text => $text,
    };
};

sub _para {
    my ( $self, $json ) = @_;
    $json->[ 0 ]->{ t } eq 'Para'
        or $self->_e( "oops", $json->[ 0 ] );
    my $spans = $self->_spans( $json->[ 0 ]->{ c } );
    shift( @$json );
    return {
        type  => 'para',
        lang  => $self->{ lang },
        spans => $spans,
    };
};

sub _ulist {
    my ( $self, $json ) = @_;
    $json->[ 0 ]->{ t } eq 'BulletList'
        or $self->_e( "oops", $json->[ 0 ] );
    my $items = $self->_items( $json->[ 0 ]->{ c } );
    shift( @$json );
    return {
        type  => 'ulist',
        lang  => $self->{ lang },
        items => $items,
    };
};

sub _olist {
    my ( $self, $json ) = @_;
    #   $json->[ 0 ] = {
    #       't' => 'OrderedList',
    #       'c' => [
    #           [
    #               $start,
    #               { 't' => $number_type },    # e. g. 'Decimal', 'LowerRoman', etc.
    #               { 't' => $delimiter_type }, # e. g. 'Period', 'OneParen', etc.
    #           ],
    #           [
    #               ... # list items
    #           ],
    #       ],
    #   };
    $json->[ 0 ]->{ t } eq 'OrderedList'
        or $self->_e( "oops", $json->[ 0 ] );
    $json->[ 0 ]->{ c }->[ 0 ]->[ 0 ] == 1
        or $self->_e( "Ordered list must start with item one", $json->[ 0 ] );
    my $items = $self->_items( $json->[ 0 ]->{ c }->[ 1 ] );
    shift( @$json );
    return {
        type  => 'olist',
        lang  => $self->{ lang },
        items => $items,
    };
};

sub _items {
    my ( $self, $json ) = @_;
    my $items = [];
    for ( @$json ) {
        push( @$items, $self->_item( $_ ) );
    };
    @$items > 1 or $self->_e( "Single-item list" );
    return $items;
};

sub _item {
    my ( $self, $json ) = @_;
    my $block;
    if ( @$json and $json->[ 0 ]->{ t } eq 'Para' ) {
        $block = $self->_para( $json );
        not @$json
            or $self->_e( "Multi-paragraph list items are not allowed", $json->[ 0 ] );
    } elsif ( @$json and $json->[ 0 ]->{ t } eq 'Plain' )  {
        # Compact list.
        $block = {
            type  => 'para',
            spans => $self->_spans( $json->[ 0 ]->{ c } ),
        };
    };
    return $block;
};

sub _spans {
    my ( $self, $json ) = @_;
    my $spans = [];
    while ( @$json ) {
        my $span = $self->_span( $json );
        if ( @$spans and $spans->[ -1 ]->{ type } eq 'plain' and $span->{ type } eq 'plain' ) {
            # Concatenate two subsequent spans of 'plain' type.
            $spans->[ -1 ]->{ text } .= $span->{ text };
        } else {
            push( @$spans, $span );
        };
    };
    return $spans;
};

sub _span {
    my ( $self, $json ) = @_;
    my ( $type, $text, $spans );
    if ( $json->[ 0 ]->{ t } eq 'Str' ) {
        $type = 'plain';
        $text = $json->[ 0 ]->{ c };
        shift( @$json );
    } elsif ( $json->[ 0 ]->{ t } eq 'Space' ) {
        $type = 'plain';
        $text = ' ';
        shift( @$json );
    } elsif ( $json->[ 0 ]->{ t } eq 'SoftBreak' ) {
        $type = 'plain';
        $text = ' ';
        shift( @$json );
    } elsif ( $json->[ 0 ]->{ t } eq 'Quoted' ) {
        $type  = lc( $json->[ 0 ]->{ c }->[ 0 ]->{ t } );
        $spans = $self->_spans( $json->[ 0 ]->{ c }->[ 1 ] );
        shift( @$json );
    } elsif ( $json->[ 0 ]->{ t } eq 'Emph' ) {
        $type  = 'em';
        $spans = $self->_spans( $json->[ 0 ]->{ c } );
        shift( @$json );
    } elsif ( $json->[ 0 ]->{ t } eq 'Strong' ) {
        $type  = 'strong';
        $spans = $self->_spans( $json->[ 0 ]->{ c } );
        shift( @$json );
    } elsif ( $json->[ 0 ]->{ t } eq 'Code' ) {
        $type = 'code';
        $text = $json->[ 0 ]->{ c }->[ 1 ];
        shift( @$json );
    } elsif ( $json->[ 0 ]->{ t } eq 'RawInline' ) {
        $self->_e( "HTML tags are not allowed", $json->[ 0 ] );
    } else {
        $self->_e( "Unexpected span type", $json->[ 0 ] );
    };
    return {
        type  => $type,
        text  => $text,
        spans => $spans,
    };
};

}; # package News::JSON

#   ------------------------------------------------------------------------------------------------
#   News::XML — convert internal representation to XML
#   ------------------------------------------------------------------------------------------------

package News::XML {

sub new {
    my ( $class, %args ) = @_;
    my $self = {
        %args
    };
    $self->{ limit } //= $self->{ news }->{ releases }->@* + 0;
    return bless( $self, $class );
};

sub xml {
    my ( $self ) = @_;
    return __indent( 4, $self->_news( $self->{ news } ) );
};

sub _news {
    my ( $self, $news ) = @_;
    return __belem(
        'releases',
        map(
            { $self->_release( $news->{ releases }->[ $_ ] ) }
            0 .. $self->{ limit } - 1
        ),
    );
};

sub _release {
    my ( $self, $release ) = @_;
    return __belem(
        'release',
        $release->{ header },
        (
            $self->{ url }
            ?
                __ielem(
                    'url',
                    __encode( $self->{ url } . '#' . $release->{ header }->{ version } ),
                ) . "\n"
            :
                ()
        ),
        __belem(
            'description',
            map( { $self->_block( $_ ) } $release->{ description }->@* ),
        ),
    );
};

sub _block {
    my ( $self, $block ) = @_;
    my $_block;
    if ( $block->{ type } eq 'para' ) {
        $block = __belem( 'p', { lang => $block->{ lang } }, $self->_spans( $block->{ spans } ) );
    } elsif ( $block->{ type } eq 'ulist' ) {
        $block = __belem( 'ul', { lang => $self->{ lang } }, $self->_items( $block->{ items } ) );
    } elsif ( $block->{ type } eq 'olist' ) {
        $block = __belem( 'ol', { lang => $self->{ lang } }, $self->_items( $block->{ items } ) );
    } elsif ( $block->{ type } eq 'comm' ) {
        $block = '';
    } else {
        die "Oops: Unknown block type: ‘$block->{ type }’\n";
    };
    return $block;
};

sub _items {
    my ( $self, $items ) = @_;
    return join( '', map( { $self->_item( $_ ) } @$items ) );
};

sub _item {
    my ( $self, $item ) = @_;
    $item->{ type } eq 'para'
        or die "Oops: Unexpected block type in a list item: ‘$item->{ type }’\n";
    return __belem(
        'li',
        map( { $self->_span( $_ ) } $item->{ spans }->@* ),
    );
};

sub _spans {
    my ( $self, $spans ) = @_;
    return join( '', map( { $self->_span( $_ ) } @$spans ) );
};

sub _span {
    my ( $self, $span ) = @_;
    if ( $span->{ type } eq 'plain' ) {
        return __encode( $span->{ text } );
    } elsif ( $span->{ type } eq 'doublequote' ) {
        return "“" . $self->_spans( $span->{ spans } ) . "”";
    } elsif ( $span->{ type } eq 'strong' ) {
        return __ielem( 'em', $self->_spans( $span->{ spans } ) );
    } elsif ( $span->{ type } eq 'em' ) {
        return __ielem( 'em', $self->_spans( $span->{ spans } ) );
    } elsif ( $span->{ type } eq 'code' ) {
        return __ielem( 'code', __encode( $span->{ text } ) );
    } else {
        die "Oops: Unknown inline element type: ‘$span->{ type }’\n";
    };
};


# Creates block element, see __elem.
sub __belem($@) {
    return __elem( "\n", @_ );
};

# Creates inline element, see __elem.
sub __ielem($@) {
    return __elem( "", @_ );
};

=begin comment

    Create an XML element.

    Parameters:

    *   $lf (string) should be either empty string or line feed character ("\n"). If $lf is empty
        string, inline element will be created. If $lf is line feed, block elemet will be created.
        The only difference between inline and block elements is formatting:

            <elem>content</elem>    <!-- inline element -->
            <elem>                  <!-- block element -->
                content
            </elem>

    *   $name (string) — the name if XML element. Will be used to generate open and closing tags:

            <$name>...</$name>

        Name must consist of lowercase Latin letters only.

    *   $attrs (hasref, optional) — attributes.

    *   @content — the element content:

        <$name>...content will be written here as-is...</$name> <!-- inline -->
        <$name>                                                 <!-- block -->
            ...content will be written here indented...
        </$name>

    Returns:

    *   String representation of the XML element.

    Examples:

        __elem( "", 'em', "phrase to emphasize" ); # → "<em>phrase to emphasize</em>".
        __elem( "\n", 'p', "long sentence..." );   # → "<p>/\n    long sentence...\n</p>"
        __elem( "\n", 'p', { class => "hidden" }, "long sentence..." );
            # → "<p class=\"hidden\">/\n    long sentence...\n</p>"
        __elem( "\n", 'p',
            __elem( '', 'em' "Tapper" ),
            " is a keyboard layout switcher.",
        );  # → "<p>\n    <em>Tapper</em> is a keyboard layout switcher.\n</p>"

=end comment
=cut

sub __elem($$@) {
    my ( $lf, $name, @content ) = @_;
    $lf =~ m{\A\n?\z}
        or die "Oops: lf argument must be either empty string or line feed character\n";
    $name =~ m{\A[a-z]+\z}
        or die "Oops: Bad element name: ‘$name’\n";
    my $attrs;
    if ( @content and ref( $content[ 0 ] ) eq 'HASH' ) {
        $attrs = shift( @content );
        for ( keys( %$attrs ) ) {
            $_ =~ m{\A[a-z]+\z}
                or die "Oops: Bad attribute name: ‘$_’\n";
        };
    };
    my $elem =
        "<$name" .
            join(
                '',
                map(
                    { " $_=\"" . __encode( $attrs->{ $_ }, '"' ) . "\"" }
                    sort( grep( { defined( $attrs->{ $_ } ) } keys( %$attrs ) ) )
                )
            ) .
        ">$lf" .
        __indent( $lf ? 4 : 0, join( '', @content ) ) =~ s{\n\z}{}r .
        "$lf</$name>$lf";
    return $elem;
};

sub __indent($$) {
    my ( $size, $text ) = @_;
    if ( $size > 0 ) {
        my $sp = ' ' x $size;
        $text =~ s{^}{$sp}mg;
    };
    return $text;
};

sub __encode($;$) {
    my ( $text, $chars ) = @_;
    return HTML::Entities::encode_entities( $text, '<>&' . ( $chars // '' ) );
};

}; # package News::XML

#   ------------------------------------------------------------------------------------------------

sub help {
    Pod::Usage::pod2usage(
        -output  => \*STDOUT,
        -exitval => 0,
        @_,
    );
};

my $url;
my $output;
my $limit = 5;
Getopt::Long::GetOptions(
    "url=s"      => \$url,
    "o|output=s" => \$output,
    "limit=i"    => \$limit,
    "usage"      => sub { help( -verbose => 0 ); },
    "help"       => sub { help( -verbose => 1 ); },
    "man"        => sub { help( -verbose => 2 ); },
);
my @input = @ARGV ? @ARGV : ( undef );
defined( $url )
    or warn( "$0: warning: --url is not specified, <url> elements will not be generated\n" );

my $news = News->new( url => $url, limit => $limit );
for my $input ( @input ) {
    $news->load_json( $input );
};
$news->save_xml( $output );

=head1 NAME

news-to-releases.pl — Convert F<NEWS.json> to F<releases.xml>.

=head1 SYNOPSIS

    news-to-releases.pl [OPTION…] [INPUT…]

=head1 OPTIONS

=over

=item B<-o> I<OUTPUT>

=item B<--output=>I<OUTPUT>

The path of file to write result to. If no I<OUTPUT> is specified, the result will be written
to stdout.

=item B<--url=>I<URL>

If present, C<release> elements in XML file will have C<url> children with content
"I<URL>#I<version>".

=back

=head1 ARGUMENTS

=over

=item I<INPUT>

The path of JSON file to read. If no input files are specified, the script will read stdin. Input
file is expected to be result of C<pandoc -f markdown -f json> command.

=back

=head1 DESCRIPTION

The script is a part of pipeline for moving information from one or more F<NEWS.md> files to
F<metainfo.xml> file.

First, convert F<NEWS.md> files to JSON format with F<pandoc>:

    $ pandoc -f markdown -t json -o NEWS.en.json NEWS.en.md
    $ pandoc -f markdown -t json -o NEWS.ru.json NEWS.ru.md

Then convert JSON files to XML format with B<news-to-releases.pl>:

    $ news-to-releases.pl --url=https://example.org/news.html -o releases.xml \
        NEWS.en.json NEWS.ru.json

Finally, insert the F<releases.xml> file to your F<I<ID>.metainfo.xml> file.

=head2 F<NEWS.md> file format

F<NEWS.md> file is expected to look like:

    <!-- HTML/XML comments are allowed and ignored. -->

    NEWS
    ====

    One or more introductionary paragraphs. The first level header (“NEWS”) and introductionary
    paragraphs are ignored.

    The second level header must follow the pattern: “‹version› @ ‹date›”, where ‹version› is
    dot-separated series of numbers, and ‹date› is release date in format “yyyy-mm-dd”, e. g.:

    1.0.1 @ 2022-10-01
    ------------------

    Release decription: one or more paragraphs. Text may be formatted as *italic* and `code`;
    **bold** is allowed, but in XML it will be converted to italic (since there is no bold in
    AppStream).

    Release description may include unordered compact lists:

    *   an item
    *   another item

    Loose lists are allowed, too (but in AppStream there is no difference between compact and loose
    lists):

    *   But a list item must be a single paragraph. Multi-paragraph list items are not allowed.

    *   Nested lists are not allowed, too.

    *   These are AppStream limitations.

    1.0.0 @ 2022-09-10
    ------------------

    Another release decsription. Description may include ordered lists:

    1.  the first item
    2.  the second item

    Unordered list limitations are applicable to ordered lists, too.

    <!-- Use pandoc metadata block to specify the file language. 'en' (English) language can be
    omitted, but non-English languages must be specified explicitly to merge multiple
    language-specific news files into single multi-language ‘releases.xml’. -->

    ---
    lang: en
    ...

Such a file will be converted to:

    <releases>
        <release version="1.0.1" date="2022-10-01">
            <description>
                <p>
                    Release decription: one or more paragraphs. Text may be formatted as
                    <em>italic</em> and <code>code</code>; <em>bold</em> is allowed, but in XML it
                    will be converted to italic (since there is no bold in AppStream).
                </p>
                <p>
                    Release description may include unordered compact lists:
                </p>
                <ul>
                    <li>
                        an item
                    </li>
                    <li>
                        another item
                    </li>
                </ul>
                <p>
                    Loose lists are allowed, too (but in AppStream there is no difference between
                    compact and loose lists):
                </p>
                <ul>
                    <li>
                        But a list item must be a single paragraph. Multi-paragraph list items are
                        not allowed.
                    </li>
                    <li>
                        Nested lists are not allowed, too.
                    </li>
                    <li>
                        These are AppStream limitations.
                    </li>
                </ul>
            </description>
        </release>
        <release version="1.0.0" date="2022-09-10">
            <description>
                <p>
                    Another release decsription. Description may include ordered lists:
                </p>
                <ol>
                    <li>
                        the first item
                    </li>
                    <li>
                        the second item
                    </li>
                </ol>
                <p>
                    Unordered list limitations are applicable to ordered lists, too.
                </p>
            </description>
        </release>
    </releases>

=head1 EXAMPLES

Convert F<NEWS.md> to F<releases.xml> file:

    $ pandoc -f markdown -t json -o NEWS.json NEWS.md
    $ news-to-releases.pl --url=https://example.org/news.html -o releases.xml NEWS.json

=cut

# end of file #
