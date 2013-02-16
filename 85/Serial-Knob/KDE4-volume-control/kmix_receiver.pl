#!/usr/bin/perl -w
use strict;
use Device::SerialPort;

my $port = "/dev/ttyUSB0"; # <-- find right port !
my $link = Device::SerialPort->new($port) || die("could not open port: $port - $!"); # <-- match to right com port
my $read = 0;

$link->databits(8);
$link->baudrate(9600); # <-- match to arduino settings
$link->parity("none");
$link->stopbits(1);
$| = 1; # buffers disabled

while (1) {
  $read = $link->read(1);
    
  if( $read eq "+" ) {
      # amarok 2 + KDE4: volume up
      system('qdbus org.kde.kmix /kmix/KMixWindow/actions/increase_volume com.trolltech.Qt.QAction.trigger');
    }
  if( $read eq "-" ) {
      # amarok 2 + KDE4: volume down
      system('qdbus org.kde.kmix /kmix/KMixWindow/actions/decrease_volume com.trolltech.Qt.QAction.trigger');
  }
  if( $read eq "/" ) {
      # amarok 2 + KDE4: volume down
      system('qdbus org.kde.kmix /kmix/KMixWindow/actions/mute com.trolltech.Qt.QAction.trigger');
  }

  # abuse select as a finer-granulated sleep function - limit cpu usage
  # next step of escalation: Time::HiRes
  #
  # http://stackoverflow.com/questions/896904/how-do-i-sleep-for-a-millisecond-in-perl
  #
  select(undef, undef, undef, 0.01);
}

