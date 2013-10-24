#!/bin/bash

FLASH="./main.hex"
EEPROM="./main.eep"

# fuse settings for internal RC oscillator 4.8MHz
# BOD: 2.7V

# write FUSE setting
avrdude -c avrispmkii -P usb -p attiny13 -U lfuse:w:0x79:m -U hfuse:w:0xFB:m

# write FLASH
avrdude -c avrispmkII -P usb -p attiny13 -V -B 10 -e -U flash:w:$FLASH:i

# write EEPROM
avrdude -c avrispmkII -P usb -p attiny13 -V -B 10 -U eeprom:w:$EEPROM:i

