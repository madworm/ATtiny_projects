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
      system('qdbus org.mpris.amarok /Player org.kde.amarok.Mpris1Extensions.Player.VolumeUp 1');
    }
  if( $read eq "-" ) {
      # amarok 2 + KDE4: volume down
      system('qdbus org.mpris.amarok /Player org.kde.amarok.Mpris1Extensions.Player.VolumeDown 1');
  }
  if( $read eq "/" ) {
      # amarok 2 + KDE4: volume down
      system('qdbus org.mpris.amarok /Player org.kde.amarok.Mpris1Extensions.Player.Mute');
  }
}

