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
      # amarok 2 + KDE4: volume up
	  print "+";
      system('qdbus org.kde.kmix /kmix/KMixWindow/actions/increase_volume org.qtproject.Qt.QAction.trigger > /dev/null');
  }
  if( ($readS eq "-") || ($readC eq "-") ) {
      # amarok 2 + KDE4: volume down
	  print "-";
      system('qdbus org.kde.kmix /kmix/KMixWindow/actions/decrease_volume org.qtproject.Qt.QAction.trigger > /dev/null');
  }
  if( ($readS eq "/") || ($readC eq "/")  ) {
      # amarok 2 + KDE4: volume down
	  print "M";
      system('qdbus org.kde.kmix /kmix/KMixWindow/actions/mute org.qtproject.Qt.QAction.trigger > /dev/null');
  }

  if( $readC eq "q" ) {
    ReadMode 0;
	exit;
  }

  # abuse select as a finer-granulated sleep function - limit cpu usage
  # next step of escalation: Time::HiRes
  #
  # http://stackoverflow.com/questions/896904/how-do-i-sleep-for-a-millisecond-in-perl
  #
  # select(undef, undef, undef, 0.01);
  usleep(5000);
}

