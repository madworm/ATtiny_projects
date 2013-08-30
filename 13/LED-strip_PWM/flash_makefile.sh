#!/bin/bash

HEXFILE=${1:-./main.hex}

echo -e "\nTrying to flash: $HEXFILE\n"

# fuse settings for internal RC oscillator 9.6MHz
# BOD: 4.3V
avrdude -c avrispmkii -P usb -p attiny13 -U lfuse:w:0x7A:m -U hfuse:w:0xF9:m
avrdude -c avrispmkII -P usb -p attiny13 -V -B 10 -e -U flash:w:$HEXFILE:i
