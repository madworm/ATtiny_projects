#!/bin/bash

. ./config.txt

echo -e "\n\n-:###+++--- ---+++###:-\n"
echo -e "About to flash: $HEXFILE into an $MCU\n"
echo -e "To change to 'attiny25' or 'attiny85' run the script like so: '$0 25' or '$0 85'\n\n"
echo -e "Press <ENTER> to proceed"

read

# ATtiny25/45/85 fuse settings: internal RC-oscillator 1MHz, BOD: 2.7V
#
# http://www.engbedded.com/fusecalc/
#

avrdude -c $PROGRAMMER -P $PORT -p $MCU -B 1000 -U lfuse:w:0x62:m -U hfuse:w:0xDD:m -U efuse:w:0xFF:m
avrdude -c $PROGRAMMER -P $PORT -p $MCU -B 10 -e -U flash:w:$HEXFILE:i
