#!/bin/bash

HEXFILE=${1:-./bin/Debug/source.elf.hex}

echo -e "\nTrying to flash: $HEXFILE\n"

# fuse settings for internal RC oscillator (9.6/8)MHz
# BOD: 2.7V
avrdude -c usbtiny -p attiny13 -U lfuse:w:0x6A:m -U hfuse:w:0xFB:m
avrdude -c usbtiny -p attiny13 -V -B 10 -e -U flash:w:$HEXFILE:i
