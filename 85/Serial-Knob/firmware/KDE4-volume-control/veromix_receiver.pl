#!/usr/bin/perl -w
use strict;
use Device::SerialPort;
use Time::HiRes qw(usleep);

use Term::ReadKey;
ReadMode 4;

my $port = "/dev/ttyUSB0"; # <-- find right port !
my $link = Device::SerialPort->new($port) || die("could not open port: $port - $!"); # <-- match to right com port
my $readS = "";
my $readC = "";
my $muted = 0;

$link->databits(8);
$link->baudrate(9600); # <-- match to arduino settings
$link->parity("none");
$link->stopbits(1);
$| = 1; # buffers disabled

print "\n\n Press 'q' to exit\n\n";

while (1) {
  $readS = $link->read(1);
  $readC = ReadKey(-1) || "";

  if( ($readS eq "+") || ($readC eq "+") ) {
	print "+ not yet implemented\n";
	#system('dbus-send --type=method_call --dest=org.veromix.pulseaudio.qt /org/veromix/pulseaudio org.veromix.pulseaudio. > /dev/null');
  } 
  
  elsif( ($readS eq "-") || ($readC eq "-") ) {
	print "- not yet implemented\n";
	#system('dbus-send --type=method_call --dest=org.veromix.pulseaudio.qt /org/veromix/pulseaudio org.veromix.pulseaudio. > /dev/null');
  } 
  
  elsif( ( ($readS eq "/") || ($readC eq "/") ) && ( $muted == 0 ) ) {
	print "M";
	system('dbus-send --type=method_call --dest=org.veromix.pulseaudio.qt /org/veromix/pulseaudio org.veromix.pulseaudio.sink_mute uint16:0 uint16:1 > /dev/null');
	$muted = 1;
  }

  elsif( ( ($readS eq "/") || ($readC eq "/") ) && ( $muted == 1 ) ) {
	print "m";
	system('dbus-send --type=method_call --dest=org.veromix.pulseaudio.qt /org/veromix/pulseaudio org.veromix.pulseaudio.sink_mute uint16:0 uint16:0 > /dev/null');
	$muted = 0;
  }
  
  elsif( $readC eq "q" ) {
	ReadMode 0;
	exit;
  }

  # abuse select as a finer-granulated sleep function - limit cpu usage
  # next step of escalation: Time::HiRes
  #
  # http://stackoverflow.com/questions/896904/how-do-i-sleep-for-a-millisecond-in-perl
  #
  # select(undef, undef, undef, 0.01);
  usleep(50000);
}

