#!/bin/bash

HEXFILE=${1:-./bin/Debug/source.elf.hex}

echo -e "\nTrying to flash: $HEXFILE\n"

# fuse settings for external crystal 8MHz and up
# BOD: 2.7V
avrdude -c usbtiny -p attiny24 -U lfuse:w:0xE2:m -U hfuse:w:0xDD:m -U efuse:w:0xFF:m
avrdude -c usbtiny -p attiny24 -V -B 2 -e -U flash:w:$HEXFILE:i
