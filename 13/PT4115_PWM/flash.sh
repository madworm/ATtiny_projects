#!/bin/bash

HEXFILE=${1:-./bin/Debug/source.elf.hex}

echo -e "\nTrying to flash: $HEXFILE\n"

# fuse settings for internal RC oscillator 4.8MHz
# BOD: 2.7V
avrdude -c avrispmkii -P usb -p attiny13 -U lfuse:w:0x79:m -U hfuse:w:0xFB:m
avrdude -c avrispmkII -P usb -p attiny13 -V -B 10 -e -U flash:w:$HEXFILE:i
