#!/usr/bin/perl
use strict;
use warnings;
sub max {
	return $_[0] > $_[1] ? $_[0] : $_[1];
}

sub similarity {
	my ($a, $b, $n) = (0, 0, 0);
	my %a = map { $_ => 1 } @{$_[0]};
	delete $a{$_} for keys %{$_[2]};
	$a = scalar keys %a;
	for (@{$_[1]}) {
		next if $_[2]->{$_};
		$b++;
		$n++ if $a{$_};
	}
	return $n / max($a, $b);
}

my %D;
while (<>) {
	chomp;
	my ($name, @releases) = split /\s+/, $_;
	$D{$name} = \@releases;
}

my $addons = { 'os-conf' => 1 };
my @names = sort keys %D;
for (my $i = 0; $i < @names; $i++) {
	next unless $names[$i];
	my $set = [$names[$i]];
	for (my $j = $i + 1; $j < @names; $j++) {
		if (similarity($D{$names[$i]}, $D{$names[$j]}, $addons) > 0.66) {
			push @$set, $names[$j];
			$names[$j] = undef;
		}
	}
	print "SET:\n";
	print " - $_\n" for @$set;
	print "\n\n";
}
