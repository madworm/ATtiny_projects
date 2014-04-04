#!/bin/bash

FLASH=`find -iname "*.hex"`
EEPROM=`find -iname "*.eep"`

# write FUSE settings: internal RC oscillator 4.8MHz, BOD: 2.7V
avrdude -c avrispmkii -P usb -p attiny13 -U lfuse:w:0x79:m -U hfuse:w:0xFB:m

# write FLASH
avrdude -c avrispmkII -P usb -p attiny13 -V -B 10 -e -U flash:w:$FLASH:i

# write EEPROM
avrdude -c avrispmkII -P usb -p attiny13 -V -B 10 -U eeprom:w:$EEPROM:i

