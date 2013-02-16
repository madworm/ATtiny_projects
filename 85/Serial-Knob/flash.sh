#!/bin/bash

clear

HEXFILE=${1:-./bin/Debug/source.elf.hex}

echo -e "\nTrying to flash: $HEXFILE\n"

# ATtiny85 fuse settings for internal RC-oscillator 8MHz
# BOD: 2.7V

echo -e "Press <ENTER> to proceed\n"

read

avrdude -c avrispmkii -P usb -p attiny85 -B 1000 -U lfuse:w:0xE2:m -U hfuse:w:0xDD:m -U efuse:w:0xFF:m
avrdude -c avrispmkii -P usb -p attiny85 -B 1 -e -U flash:w:$HEXFILE:i
