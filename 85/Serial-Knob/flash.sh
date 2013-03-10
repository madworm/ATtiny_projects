#!/bin/bash

HEXFILE="./bin/Debug/source.elf.hex"
MCU="attiny${1:-45}"

echo -e "\n\n-:###+++--- ---+++###:-\n"
echo -e "About to flash: $HEXFILE into an $MCU\n"
echo -e "To change to 'attiny25' or 'attiny85' run the script like so: '$0 25' or '$0 85'\n\n"
echo -e "Press <ENTER> to proceed"

read

# ATtiny25/45/85 fuse settings: internal RC-oscillator 8MHz, BOD: 2.7V
#
# http://www.engbedded.com/fusecalc/
#

avrdude -c avrispmkii -P usb -p $MCU -B 1000 -U lfuse:w:0xE2:m -U hfuse:w:0xDD:m -U efuse:w:0xFF:m
avrdude -c avrispmkii -P usb -p $MCU -B 1 -e -U flash:w:$HEXFILE:i
