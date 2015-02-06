#!/usr/bin/perl
my $ts = time;
print "[$ts] SCHEDULE_SERVICE_CHECK;host.example.com;foo$_;$ts\n" for 1 .. 963;
print "[$ts] SCHEDULE_SERVICE_CHECK;host;service;$ts\n";
