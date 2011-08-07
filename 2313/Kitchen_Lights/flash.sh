#!/bin/bash

HEXFILE=${1:-./bin/Debug/source.elf.hex}

echo -e "\nTrying to flash: $HEXFILE\n"

# INT 8MHz RC OSC, 2.7V BOD
avrdude -c usbtiny -p attiny2313 -U lfuse:w:0xE4:m -U hfuse:w:0xDB:m -U efuse:w:0xFF:m
avrdude -c usbtiny -p attiny2313 -V -B 1 -e -U flash:w:$HEXFILE:i

# fuse settings for external crystal 8MHz and up
# BOD: 2.7V
#avrdude -c usbtiny -p attiny4313 -U lfuse:w:0xFF:m -U hfuse:w:0xDB:m -U efuse:w:0xFF:m
#avrdude -c usbtiny -p attiny4313 -V -B 2 -e -U flash:w:$HEXFILE:i
