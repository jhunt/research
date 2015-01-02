#!/usr/bin/perl

use strict;
use warnings;

use YAML::XS qw/LoadFile/;
use List::Util qw/max/;

my $lst = eval { LoadFile "opcodes.yml" }
	or die "Failed to load opcoes.yml: $!\n";

my $n = 0;
my $len = 0;
for my $op (@$lst) {
	my ($key, $help) = %$op;
	$len = max($len, length($key));
	$n++;
}
my $digits = int(log($n) / log(16)) + 3;

$n = 0;
print "/** OPCODES CONSTANTS **/\n";
for my $op (@$lst) {
	my ($key, $help) = %$op;
	printf "#define %-${len}s  %#0${digits}x  /* %s */\n", uc($key), $n++, $help;
}

print "\n\n";
print "/** OPCODE MNEMONIC NAMES **/\n";
print "static const char * OPCODES[] = {\n";
$n = 0;
for my $op (@$lst) {
	my ($key, $help) = %$op;
	printf qq(\t%-@{[$len+3]}s /* %-${len}s  %@{[$digits-2]}i  %#0${digits}x */\n), qq("$key",), uc($key), $n, $n;
	$n++;
}
print "\tNULL,\n";
print "};\n";
