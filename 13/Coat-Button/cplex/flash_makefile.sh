#!/bin/bash

FLASH="./main.hex"
EEPROM="./main.eep"

# write FUSE settings: internal RC oscillator 4.8MHz, BOD: 2.7V
avrdude -c avrispmkii -P usb -p attiny13 -U lfuse:w:0x79:m -U hfuse:w:0xFB:m

# write FUSE settings: internal RC oscillator 4.8MHz, BOD: OFF
#avrdude -c avrispmkii -P usb -p attiny13 -U lfuse:w:0x79:m -U hfuse:w:0xFF:m

# write FLASH
avrdude -c avrispmkII -P usb -p attiny13 -B 10 -e -U flash:w:$FLASH:i

# write EEPROM
#avrdude -c avrispmkII -P usb -p attiny13 -B 10 -U eeprom:w:$EEPROM:i

