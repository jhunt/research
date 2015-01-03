#!/usr/bin/perl

use strict;
use warnings;

use YAML::XS qw/LoadFile/;
use List::Util qw/max/;

my $lst = eval { LoadFile "opcodes.yml" }
	or die "Failed to load opcoes.yml: $!\n";

for my $op (@$lst) {
	my ($key, $o) = %$op;
	if (!$o->{constant}) {
		$o->{constant} = uc($key);
		$o->{constant} =~ s/\./_/g;
		$o->{constant} =~ s/\?$/_P/g;
	}
}

my $n = 0;
my $len = 0;
for my $op (@$lst) {
	my ($key, $o) = %$op;
	$len = max($len, length($o->{constant}));
	$n++;
}

$n = 0;
print "/** OPCODE CONSTANTS **/\n";
for my $op (@$lst) {
	my ($key, $o) = %$op;
	next if $o->{sugar};
	printf "#define %-${len}s  %#04x  /* %s */\n", $o->{constant}, $n++, $o->{help};
}

print "\n\n";
print "#ifdef OPCODES_EXTENDED\n";
print "/** OPCODE MNEMONIC NAMES **/\n";
print "static const char * OPCODES[] = {\n";
$n = 0;
for my $op (@$lst) {
	my ($key, $o) = %$op;
	next if $o->{sugar};
	printf qq(\t%-@{[$len+3]}s /* %-${len}s  %2i  %#04x */\n), qq("$key",), $o->{constant}, $n, $n;
	$n++;
}
print "\tNULL,\n";
print "};\n";

print "\n\n";
print "/** ASM TOKENS **/\n";
$n = 0x40;
for my $op (@$lst) {
	my ($key, $o) = %$op;
	printf "#define T_OPCODE_%-${len}s  %#04x  /* %s */\n", $o->{constant}, $n++, $o->{help};
}

print "\n\n";
print "static const char * ASM[] = {\n";
$n = 0;
for my $op (@$lst) {
	my ($key, $o) = %$op;
	printf qq(\t%-@{[$len+3]}s /* T_OPCODE_%-${len}s  %2i  %#04x */\n), qq("$key",), $o->{constant}, $n, $n;
	$n++;
}
print "\tNULL,\n";
print "};\n";
print "#endif\n";
