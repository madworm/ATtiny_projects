#!/bin/bash

clear

HEXFILE=${1:-./bin/Debug/source.elf.hex}

echo -e "\nTrying to flash: $HEXFILE\n"

# ATtiny85 fuse settings for external crystal 8MHz
# BOD: 2.7V

echo -e "Cover IR-tight (put your hand over it) or disconnect the TSOP!\n"
echo -e "Press <ENTER> to proceed\n"

read

avrdude -c usbtiny -p attiny85 -B 1000 -U lfuse:w:0xDE:m -U hfuse:w:0xDD:m -U efuse:w:0xFF:m
avrdude -c usbtiny -p attiny85 -B 10 -e -V -U flash:w:$HEXFILE:i
