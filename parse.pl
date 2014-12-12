#!/usr/bin/perl

sub parse
{
	my ($file) = @_;
	my @smaps;
	my $this;

	open my $fh, "<", $file or return;
	while (<$fh>) {
		if (m/^([0-9a-f]+)-([0-9a-f]+) (\S+) ([0-9a-f]+) ([0-9a-f]+:[0-9a-f]+) ([0-9]+)\s+(\S+)$/) {
			$this = {
				address => { start => $1, end => $2 },
				mode    => $3,
				offset  => int($4),
				dev     => $5,
				inode   => int($6),
				path    => $7,
			};
			push @smaps, $this;
			next;
		}
		next unless $this;

		   if (m/^(\S+):\s+([0-9]+) kB$/) { $this->{usage}{lc $1} = $2 * 1024; }
		elsif (m/&(\S+):\s+(.*)$/)        { $this->{usage}{lc $1} = $2; }
	}
	close $fh;

	return @smaps;
}
sub summarize
{
	my %sum;
	for my $m (@_) {
		$sum{$m->{path}}{usage}{$_} += $m->{usage}{$_}
			for keys $m->{usage};
	}
	return \%sum;
}
sub dumbdown
{
	my %sum;
	for my $m (@_) {
		my $path = $m->{path};
		$path = '[libs]' if $path =~ m{/lib/|\.so};
		$sum{$path}{usage}{$_} += $m->{usage}{$_}
			for keys $m->{usage};
	}
	return \%sum;
}

my $pid = $ARGV[0] || $$;
my $x = dumbdown(parse "/proc/$pid/smaps");
for (sort keys %$x) {
	printf "%10.3lfk %s\n", $x->{$_}{usage}{rss} / 1024, $_;
}
