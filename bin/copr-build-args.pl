#!/usr/bin/perl

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: bin/copr-build-args.pl
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

use Path::Tiny;
use Getopt::Long;
use FindBin qw{ $Script };

sub read_args($$) {
    my ( $args, $file ) = @_;
    $file->exists or die "$Script: Config file '$file' is not found.\n";
    $file->is_file or die "$Script: Config file '$file' is not a file.\n";
    for my $line ( $file->lines_utf8( { chomp => 1 } ) ) {
        $line =~ s{#.*}{};
        $line =~ s{^\s+}{};
        $line =~ s{\s+$}{};
        $line eq '' and next;
        if ( 0 ) {
        } elsif ( $line =~ s{^!~\s*}{} ) {
            @$args = grep( { $_ !~ $line } @$args );
        } elsif ( $line =~ s{^!\s*}{} ) {
            @$args = grep( { $_ ne $line } @$args );
        } else {
            if ( not grep( { $_ eq $line } @$args ) ) {
                push( @$args, $line );
            };
        };
    };
};

GetOptions(
);

not @ARGV and die "$Script: Not enought arguments.\n";
@ARGV > 1 and die "$Script: Too many arguments.\n";

my $local = path( $ARGV[ 0 ] );

my $args = [];

# Read global config file.
my $global = $local;
for ( ; ; ) {
    $global->realpath->is_rootdir and do {
        warn "$Script: Global config file is not found.\n";
        last;
    };
    $global = $global->parent;
    $global->child( '.copr' )->exists and do {
        read_args( $args, $global->child( '.copr' ) );
        last;
    };
};

# Read local config file.
read_args( $args, $local->child( '.copr' ) );

# Print arguments.
print( "$_\n" ) for @$args;
exit( 0 );

__END__

# end of file #
