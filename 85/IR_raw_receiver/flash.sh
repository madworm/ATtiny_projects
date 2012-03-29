#!/bin/bash

HEXFILE=${1:-./bin/Debug/source.elf.hex}

echo -e "\nTrying to flash: $HEXFILE\n"

# ATtiny85 fuse settings for external crystal 8MHz
# BOD: 2.7V
avrdude -c usbtiny -p attiny85 -B 100 -U lfuse:w:0xDE:m -U hfuse:w:0xDD:m -U efuse:w:0xFF:m
avrdude -c usbtiny -p attiny85 -V -B 1 -e -U flash:w:$HEXFILE:i
