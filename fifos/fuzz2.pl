#!/usr/bin/perl
my $ts = time;
sub submit {
	open my $fh, ">", "io.fifo" or die "io.fifo: $!\n";
	print $fh $_[0];
	close $fh;
}
submit "[$ts] SCHEDULE_SERVICE_CHECK;host.example.com;foo$_;$ts\n" for 1 .. 963;
submit "[$ts] SCHEDULE_SERVICE_CHECK;host;service;$ts\n";
