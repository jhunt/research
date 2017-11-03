#!/usr/bin/perl
use strict;
use warnings;

my (@pre, @post, @rules);
my $rule = undef;
my $section = 1;
while (<>) {
	chomp;
	if (m/^----+$/) {
		$section++;
		die "syntax error (too many sections)\n" if $section > 3;
		next;
	}
	if ($section == 1) {
		push @pre, "$_\n";

	} elsif ($section == 3) {
		push @rules, $rule if $rule;
		$rule = undef;
		push @post, "$_\n";

	} else {
		if (m/^\S/) {
			push @rules, $rule if $rule;
			$rule = { regex => $_, body => [] };

		} else {
			push @{$rule->{body}}, "$_\n";
		}
	}
}
die "syntax error (no preamble)"  if $section == 1;
die "syntax error (no postamble)" if $section == 2;

my $spec = {
	pre   => join('', @pre),
	post  => join('', @post),
	rules => [ map {
		$_->{body} = join('', @{$_->{body}});
		$_;
	} @rules ]
};

use Data::Dumper;
print Dumper($spec);

my (@starts, @states);

for my $rule (@rules) {
	my $state = @states;

	$states[$state] = {};
}

print Dumper(\@starts, \@states);

