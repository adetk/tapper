#!/usr/bin/perl

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/subst.pl
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
#   see <https://www.gnu.org/licenses/>.
#
#   SPDX-License-Identifier: GPL-3.0-or-later
#
#   ---------------------------------------------------------------------- copyright and license ---

use strict;
use warnings;

sub read_file($) {
    my ( $path ) = @_;
    my $bulk;
    open( my $handle, '<', $path )
      or die "Can't read file “$path”: $!\n";
    if ( wantarray() ) {
        $bulk = [ $handle->getlines() ];
    } else {
        local $/ = undef;
        $bulk = $handle->getline();
    };
    $handle->close()
      or die "Can't read file “$path”: $!\n";
    return wantarray() ? @$bulk : $bulk;
};

my $name_re = qr{[a-z_][a-z_0-9]+}i;

my @files;          # Input files.
my %opts = (
    f => undef,     # File with variable definitions.
    o => undef,     # Output file.
);
while ( @ARGV ) {
    my $arg = shift( @ARGV );
    if ( $arg eq '--' ) {
        push( @files, @ARGV );
        @ARGV = ();
    } elsif ( $arg =~ m{\A-([a-z])\z}i ) {
        exists( $opts{ $1 } ) or die "Unknown option “$arg”\n";
        @ARGV or die "Option “$arg” requires an argument\n";
        $opts{ $1 } = shift( @ARGV );
    } elsif ( $arg =~ m{\A--.}i ) {
        die "Unknown option “$arg”\n";
    } else {
        push( @files, $arg );
    };
};

my %vars;   # Variables.

$opts{ f } or die "Option “-f” is mandatory\n";
my $file = $opts{ f };
for my $line ( read_file( $file ) ) {
    chomp( $line );
    $line =~ m{\A($name_re)=(.*)\z}
        or die "$file: $.: Invalid line: $line\n";
    my ( $name, $value ) = ( $1, $2 );
    if ( $value =~ m{\A\$\(\<\h*(.*?)\h*\)\z} ) {
        my $path = $1;
        $value = sub {
            $vars{ $name } = read_file( $path );
            return $vars{ $name };
        };
    };
    $vars{ $name } = $value;
};

my $bugs = 0;   # Number of bugs (unknown variables) in input files.

@files or die "No files specified, nothing to do\n";

my $output;
if ( $opts{ o } ) {
    open( $output, '>', $opts{ o } )
      or die "Can't write file “$opts{ o }”: $!\n";
} else {
    $output = \*STDOUT;
};

for my $file ( @files ) {
    my $input;
    if ( $file eq '-' ) {
        $input = \*STDIN;
    } else {
        open( $input, '<', $file )
          or die "Can't read file “$file”: $!\n";
    };
    while ( my $line = $input->getline() ) {
        # Delete comment with leading whitespace. If the line is empty after it, delete entire line.
        if ( $line =~ s{\h*@#@.*}{} and $line eq "\n" ) {
            next;
        };
        # Expand variables, if any.
        $line =~ s{(\@($name_re)\@)}{
            if ( not defined( $vars{ $2 } ) ) {
                ++ $bugs;
                STDERR->print( "$file: $.: Unknown variable “$2”\n" );
                $1;
            } elsif ( ref( $vars{ $2 } ) eq 'CODE' ) {
                $vars{ $2 }->();
            } else {
                $vars{ $2 };
            };
        }ge;
        $output->print( $line );
    };
    close( $input )
      or die "Can't read file “$file”: $!\n";
};

if ( $opts{ o } ) {
    close( $output )
      or die "Can't write file “$opts{ o }”: $!\n";
};

exit( $bugs ? 1 : 0 );

# end of file #
