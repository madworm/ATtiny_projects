#!/bin/bash

. ../config.txt

ARDUINO_H=$HOME/$ARDUINO_DIR/hardware/arduino/cores/arduino/Arduino.h

if [[ -e $ARDUINO_H.orig ]]
then
	mv $ARDUINO_H.orig $ARDUINO_H
	echo -e "\n  'Arduino.h' restored!\n"
else
	echo -e "\n  Nothing to do!\n"
fi

if [[ -e $HOME/$ARDUINO_DIR/Ethernet ]]
then
	mv $HOME/$ARDUINO_DIR/Ethernet $HOME/$ARDUINO_DIR/libraries	
	echo -e "\n  'Ethernet' library reactivated!\n"
else 
	echo -e "\n  Nothing to do!\n"
fi	

