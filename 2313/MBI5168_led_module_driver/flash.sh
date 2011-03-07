#!/bin/bash

#avrdude -c usbtiny -p attiny2313 -V -B 1 -e -U flash:w:$1:i

# fuse settings for external crystal 8MHz and up
# BOD: 2.7V
avrdude -c usbtiny -p attiny4313 -U lfuse:w:0xff:m -U hfuse:w:0xdb:m -U efuse:w:0xff:m
avrdude -c usbtiny -p attiny4313 -V -B 2 -e -U flash:w:$1:i
