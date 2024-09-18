#!/usr/bin/perl

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/manifest.pl
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

=encoding UTF-8
=cut

use strict;
use warnings;
use utf8;
use open IO => ':locale';

use Getopt::Long;
use Path::Tiny;
use Pod::Usage;
#   Try::Tiny used in Manifest.

#   ------------------------------------------------------------------------------------------------

=begin comment

`Path::Tiny` exception looks like:

    Error open (<:unix) on 'MANIFEST.md': No such file or directory at bin/manifest.pl line 114.

Printing the source file and line number is confusing since it is run time error, not a bug. The
script is correct, so there is no reason to print source file and line number. Let's fix the
message, so it looks like:

    MANIFEST.md: Can't open (<:unix): No such file or directory

=end comment
=cut

{
    no warnings qw( redefine );
    *Path::Tiny::Error::throw = sub {
        my ( $class, $op, $file, $err ) = @_;
        chomp( my $trace = $Carp::Verbose ? Carp::shortmess : '' );
        my $msg = "$file: Can't $op: $err$trace\n";
        die bless( { op => $op, file => $file, err => $err, msg => $msg }, $class );
    };
};

#   ------------------------------------------------------------------------------------------------

package Error {

    use overload '""' => sub {
        my ( $self ) = @_;
        return $self->{ msg };
    };

    sub new {
        my ( $class, $err, $file, $line, $text ) = @_;
        chomp( $err );
        my $self = {
            err  => $err,
            file => $file,
            line => $line,
            text => $text,
        };
        if ( defined $text ) {
            $self->{ msg } = sprintf( "%s:%d: %s:\n>   %s\n", $file, $line, $err, $text );
        } elsif ( defined $line ) {
            $self->{ msg } = sprintf( "%s:%d: %s\n", $file, $line, $err );
        } elsif ( defined $file ) {
            $self->{ msg } = sprintf( "%s: %s\n", $file, $err );
        } else {
            $self->{ msg } = sprintf( "%s\n", $err );
        };
        return bless( $self, $class );
    };

    sub throw {
        if ( ref( $_[ 0 ] ) ) {
            die $_[ 0 ];
        } else {
            my $class = shift( @_ );
            $class->new( @_ )->throw();
        };
    };
};

#   ------------------------------------------------------------------------------------------------

package Manifest {

use Try::Tiny;

our %Ignore = (
    # SCM     => ignore file.
    auto      => undef,
    fossil    => '.fossil-settings/ignore-glob',
    mercurial => '.hgignore',
);

sub new {
    my ( $class, $file, @opts ) = @_;
    my $self = {
        file => $file,
        opts => { @opts },
    };
    bless( $self, $class );
    $self->read();
    return $self;
};

sub read {
    my ( $self ) = @_;
    my $errors = 0;                     # Number of errors found so far.
    my $bulk = $self->{ file }->slurp_utf8();
    # Drop comments, but keep line feed characters intact to preserve line numbers:
    $bulk =~ s{(<!--.*?-->)}{ $1 =~ tr{\n}{ }cr }gse;
    my @text = split( m{\n}, $bulk );
    my $line = 0;
    my $content = [];
    my %files;
    for my $text ( @text ) {
        ++ $line;
        my $error = sub {
            my ( $msg ) = @_;
            my $e = Error->new( $msg, $self->{ file }, $line, $text );
            STDERR->print( $e );
            ++ $errors;
        };
        # Skip empty and blank lines:
        if ( $text =~ m{\A\h*\z} ) {
            next;
        };
        # Skip paragraph lines:
        if ( $text =~ m{\A\S} ) {
            next;
        };
        # Parse indented lines:
        if ( $text =~ m{\A    (\S+)(?:\h+(.*?)\h*)?\z} ) {
            my ( $file, $license ) = ( Path::Tiny::path( $1 ), $2 // '' );
            if ( $files{ "$file" } ) {
                $error->( "File listed more than once" );
                next;
            };
            $files{ "$file" } = 1;
            if ( $file->is_absolute ) {
                $error->( "Absolute path is not allowed" );
                next;
            };
            if ( "$file" =~ m{(?:\A|/)\.\.?(?:/|\z)} ) {
                $error->( "'.' and '..' are not allowed" );
                next;
            };
            push( @$content, [ $line, $file, $license ] );
        } else {
            $error->( "Can't parse line" );
            next;
        };
    };
    if ( $errors ) {
        $self->{ errors } = $errors;
        Error->throw(
            $errors == 1
                ? $errors . " error found in the manifest"
                : $errors . " errors found in the manifest",
            $self->{ file }
        );
    };
    $self->{ content } = $content;
    return $self;
};

sub list {
    my ( $self ) = @_;
    for my $item ( $self->{ content }->@* ) {
        STDOUT->print( $item->[ 1 ], "\n" );
    };
    return $self;
};

sub licenses {
    my ( $self ) = @_;
    if ( not $self->{ licenses } ) {
        my $licenses = {};
        for my $file ( $self->path( 'LICENSES' )->children ) {
            my $license = $file->basename( qw{ .md .reuse } );
            $licenses->{ $license } = $file;
        };
        $self->{ licenses } = $licenses;
    };
    return $self->{ licenses };
};

# Parse Mercurial ignore file.
sub _ignore_mercurial {
    my ( $self ) = @_;
    my $file = $self->path( $Ignore{ mercurial } );
    my @ignore;
    my $line = 0;   # line number
    for my $text ( $file->lines_utf8( { chomp => 1 } ) ) {
        ++ $line;
        if ( $text =~ m{\A\h*(?:#.*)?\z} ) {
            next;
        };
        if ( $text =~ m{\Asyntax:\h*glob\h*\z} ) {
            Error->throw( "glob syntax is not supported (yet?)", $file, $line, $text );
        };
        if ( $text =~ m{\Asyntax:\h*regexp\h*\z} ) {
            next;
        };
        try {
            push( @ignore, qr{$text}s );
        } catch {
            my $ex = $_;
            if ( "$ex" =~ m{\A(.*) in regex; marked by <-- HERE in .*$} ) {
                Error->throw( $1, $file, $line, $text );
            };
            die $ex;
        };
    };
    push( @ignore, qr{\A\.hg\z}, qr{\A\.hgtags\z} );
    my $ignore = join( '|', @ignore );
    return qr{$ignore};
};

# Convert Fossil pattern to regexp.
sub _fossil_pattern {
    my ( $self, $pattern ) = @_;
    my $regexp = '';
    pos( $pattern ) = 0;
    while ( $pattern =~ m{\G(?=.)}cgs ) {
        if ( $pattern =~ m{\G\*}cgs ) {
            $regexp .= '.*';
        } elsif ( $pattern =~ m{\G\?}cgs ) {
            $regexp .= '.';
        } elsif ( $pattern =~ m{\G(\[\^?\]?[^]]*\])}cgs ) {
            $regexp .= $1;
        } elsif ( $pattern =~ m{\G([^[*?]+)}cgs ) {
            $regexp .= quotemeta( $1 );
        } else {
            Error->throw( "Bad glob pattern (unclosed character range?)" );
        };
    };
    if ( $regexp eq '' ) {
        return ();
    } else {
        #   If regexp compilation failed, the error message looks like
        #       ... in regex; marked by <-- HERE in m/... <-- HERE .../ at bin/manifest.pl line 229.
        #   The error message cointains source file and line. It is confusing, because actual regexp
        #   ig in ignore file, not in the script. Let's fix it:
        try {
            $regexp = qr{\A$regexp\z}s;
        } catch {
            my $ex = $_;
            if ( "$ex" =~ m{\A(.*?) in regex; marked by <-- HERE in .*$} ) {
                Error->throw( $1 );
            };
            die $ex;
        };
        return ( $regexp, );
    };
};

=begin comment

Parse Fossil ignore file.

Fossil glob patterns described there: <https://www2.fossil-scm.org/home/doc/trunk/www/globs.md>.

Patterns are separated either by white spaces (HT, LF, VT, FF, CR, SP) or by commas. If pattern
contains white spaces or commas, it must be quoted with either single or double quotation marks.
Fossil documentation does not define how to interpret partially quoted patterns, e. g.:

    'ab'c
    a'b'c
    ab'c'

To avoid different interpretation, let's prohibit such constructs. Let's assume the Fossil ignore
glob file syntax is:

    ignore file content   = [ separator ], { pattern, separator }, [ pattern ];
    pattern               = quoted pattern | unquoted pattern;
    quoted pattern        = single quoted pattern | double quoted pattern;
    signle quoted pattern = single quote, { any character - single quote }, single quote
    double quoted pattern = double quote, { any character - double quote }, double quote
    unquoted pattern      = { any character - ( comma | space | single quote | double quote ) };
    separator             = ( space | comma ), { space | comma };
    space                 = " " | HT | LF | VT | FF | CR;
    comma                 = ",";

Btw, Fossil does not provide any way to put comments to ignore file. However, quoted patterns can
be used instead, e. g.:

    "
        Comment…
    "

It very unlikely that file with such a name (`"\n    Comment…\n"`) will appear in the project tree.
The longer comment, the less likely such file will appear. Additionally, to distinguish such
pseudo-comments from regular quoted patterns, let's start comments with the hash character:

    "#
        Comment…
    "

Fossil will treat such construct as a pattern anyway, but the script will recognize comment and
ignore it.

=end comment
=cut

sub _ignore_fossil {
    my ( $self ) = @_;
    my $file = $self->path( $Ignore{ fossil } );
    my $text = $file->slurp_utf8();
    my $mark = "◆";                 # Special character to denote error position in text.
    my $ws = "\t\n\cK\f\r ";        # Whitespace: HT, LF, VT, FF, CR, SP.
    my $error = sub {
        my ( $err ) = @_;
        my $pos = pos( $text );     # Error position in `$text`.
        my $line = ( substr( $text, 0, $pos ) =~ tr/\n/\n/ ) + 1;   # Error line number.
        # `\G` fails to match if the current position is behind the end of the string.
        if ( $pos < length( $text ) ) {
            $text =~ m{^(.*)\G(.*)$}cgm
                or die "oops";      # It should never occur.
            Error->throw( $err, $file, $line, "$1$mark$2" );
        } else {
            Error->throw( $err, $file, $line, ( $text =~ m{^(.*)\z}m ? $1 : '' ) . $mark );
        };
    };
    my @regexps;
    my $pattern = sub {
        my ( $pattern ) = @_;
        try {
            push( @regexps, $self->_fossil_pattern( $pattern ) );
        } catch {
            my $ex = $_;
            if ( ref( $ex ) and $ex->isa( 'Error' ) ) {
                $error->( $ex );
            };
            die $ex;
        };
    };
    $text =~ m{\G[$ws,]*}cgs or die "oops";     # Skip leading ws, if any.
    for ( ; ; ) {
        if ( pos( $text ) >= length( $text ) ) {
            last;
        };
        if ( $text =~ m{\G'#[^']*}cgs ) {
            # Skip comment.
            $text =~ m{\G'}cgs or $error->( "Closing single quotation mark (') expected" );
        } elsif ( $text =~ m{\G"#[^"]*}cgs ) {
            # Skip comment.
            $text =~ m{\G"}cgs or $error->( "Closing double quotation mark (\") expected" );
        } elsif ( $text =~ m{\G'([^']*)}cgs ) {
            $pattern->( $1 );
            $text =~ m{\G'}cgs or $error->( "Closing single quotation mark (') expected" );
        } elsif ( $text =~ m{\G"([^"]*)}cgs ) {
            $pattern->( $1 );
            $text =~ m{\G"}cgs or $error->( "Closing double quotation mark (\") expected" );
        } elsif ( $text =~ m{\G([^$ws,'"]*)}cgs ) {
            $pattern->( $1 );
        } else {
            $error->( "Oops, can't parse ignore-glob file" );
        };
        if ( pos( $text ) >= length( $text ) ) {
            last;
        };
        if ( $text =~ m{\G[$ws,]+}cgs ) {
            # Skip white space.
        } else {
            $error->( "White space or comma expected between glob patterns" );
        };
    };
    my $regexp = join( '|', @regexps );
    return qr{$regexp};
};

sub _ignore_auto {
    my ( $self ) = @_;
    my @scms = sort( grep( { $_ ne 'auto' } keys( %Ignore ) ) );
    for my $scm ( @scms ) {
        if ( $self->path( $Ignore{ $scm } )->exists ) {
            my $method = '_ignore_' . $scm;
            return $self->$method();
        };
    };
    Error->throw( "No ignore file found" );
};

sub ignore {
    my ( $self ) = @_;
    if ( not $self->{ ignore } ) {
        my $method = "_ignore_" . $self->{ opts }->{ ignore };
        my $ignore = $self->$method();
        $self->{ ignore } = $ignore;
    };
    return $self->{ ignore };
};

sub dir {
    my ( $self ) = @_;
    return $self->{ file }->parent;
};

sub path {
    my ( $self, $path ) = @_;
    if ( defined $path ) {
        return $self->{ file }->sibling( $path );
    } else {
        return $self->{ file };
    };
};

sub validate {
    my ( $self ) = @_;
    my $errors = 0;                     # Number of errors found so far.
    my $licenses = $self->licenses;     # Known licenses.
    my $proj = $self->dir->realpath;
    for my $line ( $self->{ content }->@* ) {
        my ( $n, $file, $license ) = @$line;
        my $error = sub {
            my $e = Error->new( "$file: $_[ 0 ]", $self->{ file }, $n );
            STDERR->print( $e );
            ++ $errors;
        };
        try {
            my $path = $self->path( $file );
            my $is_symlink = -l $path;
            my $is_license = $file->dirname eq "LICENSES/";
            if ( $is_license ) {
                # Files in LICENSES directory treated specially: they should not be
                # followed by license identifier.
                if ( $license ne '' ) {
                    $error->( "License is not expected for files in ‘LICENSES’ directory" );
                } elsif ( not $path->exists ) {
                    $error->( "No such file." );
                };
            } else {
                my $bulk = $path->slurp_utf8;
                if ( $bulk =~ m{\h+SPDX-License-Identifier:\h*(.*?)\h*$}m ) {
                    my $attr = $1;
                    if ( $license eq '' ) {
                        $error->( "No license specified in manifest" );
                    } elsif ( $license ne $attr ) {
                        $error->( "Manifested license ‘$license’ ≠ file license ‘$attr’" );
                    } elsif ( not $licenses->{ $license } ) {
                        $error->( "Unknown license ‘$license’" );
                    };
                } else {
                    $error->( "‘SPDX-License-Identifier’ attribute not found in the file" );
                };
                if ( $bulk =~ m{\h+File: (\S+)$}m ) {
                    my $attr = $1;
                    if ( not $is_symlink and $file ne $attr ) {
                        $error->( "‘File’ attribute ‘$attr’ ≠ actual file name" );
                    };
                } else {
                    $error->( "‘File’ attribute not found in the file" );
                };
            };
            if ( $is_symlink ) {
                if ( not $proj->subsumes( $path->realpath ) ) {
                    $error->( "Symlink points outside of the project tree." );
                };
            };
        } catch {
            my $ex = $_;
            if ( $ex->isa( 'Path::Tiny::Error' ) ) {
                $error->( "$ex->{ err }" );
            } else {
                die $ex;
            };

        };
    };
    my $check_files;
    my %files = map( { $_->[ 1 ] => 1 } $self->{ content }->@* );
    $check_files = sub {
        my ( $dir ) = @_;
        my $it = $dir->iterator();
        while ( my $path = $it->() ) {
            my $error = sub {
                my $e = Error->new( "$path: $_[ 0 ]" );
                STDERR->print( $e );
                ++ $errors;
            };
            my $rel = $path->relative( $self->dir );
            if ( "$rel" !~ $self->ignore ) {
                if ( $path->is_dir ) {
                    $check_files->( $path );
                } else {
                    if ( not $files{ "$rel" } ) {
                        $error->( "File not in manifest", $path );
                    };
                };
            };
        };
    };
    $check_files->( $self->dir );
    if ( $errors ) {
        $self->{ errors } = $errors;
        Error->throw(
            $errors == 1
                ? $errors . " error found in the manifest"
                : $errors . " errors found in the manifest",
            $self->{ file }
        );
    };
};

}; # package Manifest

#   ------------------------------------------------------------------------------------------------

sub help {
    pod2usage(
        -output  => \*STDOUT,
        -exitval => 0,
        @_,
    );
};

my $check  = 0;
my $list   = 1;
my $ignore = 'auto';
{
    local $SIG{ __WARN__ } = sub { Error->throw( $_[ 0 ], $0 ); };
    Getopt::Long::GetOptions(
        "check!"    => \$check,
        "list!"     => \$list,
        "ignore=s"  => sub {
            my ( $opt, $val ) = @_;
            exists( $Manifest::Ignore{ $val } )
                or die "Invalid --$opt option value: ‘$val’\n";
            $ignore = $val;
        },
        "usage"     => sub { help( -verbose => 0 ); },
        "help"      => sub { help( -verbose => 1 ); },
        "man"       => sub { help( -verbose => 2 ); },
    ) or Error->throw( "Command line errors found", $0 );
}
@ARGV == 0 and Error->throw( "No arguments", $0 );
@ARGV > 1 and Error->throw( "Too many arguemnts", $0 );
my $manifest = Manifest->new( path( $ARGV[ 0 ] ), ignore => $ignore );
if ( $check ) {
    $manifest->validate();
};
if ( $list ) {
    $manifest->list();
};

__END__

=head1 NAME

B<manifest.pl> — list or validate a manifest file.

=head1 SYNOPSIS

    manifest.pl [OPTION…] MANIFEST

=head1 OPTIONS

=over

=item B<--check>

Validate the manifest (perform checks that require filesystem access). By default the manifest is
not validated (but some checks that do not require filesystem access are performed anyway).

=item B<--ignore=>I<SCM>

Respect I<SCM>-specific ignore file. I<SCM> is either B<fossil> (ignore file is
F<.fossil-settings/ignore-glob>) or B<mercurial> (ignore file is F<.hgignore>).

If I<SCM> is B<auto>, the script will try to find ignore file automatically. SCMs are tried in
alphabetical order.

=item B<--list>

List all the files of the manifest. This is default behavior, use B<--no-list> option to suppress
listing.

=item B<--usage>

Print very short usage message and exit.

=item B<--help>

Print short help message and exit.

=item B<--man>

Show manual and exit.

=back

=head1 ARGUMENTS

=over

=item I<MANIFEST>

The manifest file to list and validate.

=back

=head1 DESCRIPTION

The script lists and/or validates a manifest file.

=head2 Manifest file format

A manifest file is expected to be markdown file:

    <!--
        HTML comments are allowed and ignored. Empty or blank lines are ignored too.
    -->

    MANIFEST
    ========

    Headings and text paragraphs are allowed and ignored.

    Build scripts
    -------------

    Indented blocks are recognized as file lists:

        bin/build.sh                    GPL-3.0-or-later
        bin/check-cpp-cppcheck.sh       GPL-3.0-or-later
        bin/check-desktop.sh            GPL-3.0-or-later

    The first word is treated as file path. Spaces in path are not allowed. The second word (if
    any) is treated as SPDX license identifier.

    Data files
    ----------

        data/tapper.desktop             FSFAP
        data/tapper.gschema.xml         FSFAP
        data/tapper.metainfo.xml        FSFAP

    Licenses
    --------

    Used licenses are expected to be in `LICENSES` directory.

        LICENSES/FSFAP.md
        LICENSES/GPL-3.0.md

    C++ sources
    -----------

        src/app.cpp                     GPL-3.0-or-later
        src/app.hpp                     GPL-3.0-or-later
        src/base.cpp                    GPL-3.0-or-later

    <!-- end of file -->

=head2 Listing

The script can list the manifest files: print them to standard output stream, a file per line. Only
file names are printed; HTML comments, empty and blank lines, headings, paragraphs, SPDX license
identifiers are not printed, e. g.:

    bin/build.sh
    bin/check-cpp-cppcheck.sh
    bin/check-desktop.sh
    data/tapper.desktop
    data/tapper.gschema.xml
    data/tapper.metainfo.xml
    LICENSES/FSFAP.md
    LICENSES/GPL-3.0.md
    src/app.cpp
    src/app.hpp
    src/base.cpp

=head2 Validation

The script performs the following static checks at reading a manifest:

=over

=item *

Only relative path are used.

=item *

No parent directory references (F<..>) are used.

=item *

All listed files are unique (no files are listed more than once).

=back

These checks are performed if B<--check> option is specified:

=over

=item *

Every file (except files in F<LICENSES> directory) followed by SPDX license identifier. License
files should not be followed by SPDX license identifiers.

=item *

Every file specified in the manifest exists.

=item *

Every file has SPDX license identifier inside.

=item *

License specified in the manifest matches the license specified in the file.

=item *

Every used license has corresponding file in the F<LICENSES> directory.

=item *

Every file in the directory containing the manifest and all subdirectories (exclude files listed in
SCM-specific ignore file) is listed in the manifest.

=back

=head1 EXAMPLES

List the manifest files:

    $ manifest.pl MANIFEST.md

Validate the manifest, do not list files:

    $ manifest.pl --check --no-list MANIFEST.md

Validate the manifest, respect Mercurial F<.hgignore> file; if no errors are found, list the files:

    $ manifest.pl --check --ignore=mercurial MANIFEST.md

=cut

# end of file #
