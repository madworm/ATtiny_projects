#!/bin/bash

clear

HEXFILE=${1:-./bin/Debug/source.elf.hex}

echo -e "\nTrying to flash: $HEXFILE\n"

# ATtiny85 fuse settings for external crystal 8MHz
# BOD: 2.7V

echo -e "Cover IR-tight (put your hand over it) or disconnect the TSOP!\n"
echo -e "Press <ENTER> to proceed\n"

read

# set FUSE settings
avrdude -c avrispmkii -P usb -p attiny85 -B 1000 -U lfuse:w:0xDE:m -U hfuse:w:0xDD:m -U efuse:w:0xFF:m

# write firmware
avrdude -c avrispmkii -P usb -p attiny85 -B 1 -e -U flash:w:$HEXFILE:i

# lock the chip
# avrdude -c avrispmkii -P usb -p attiny85 -B 1000 -U lock:w:0xFC:m
