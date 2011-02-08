#!/bin/bash

avr-gcc -Os -g -fno-exceptions -ffunction-sections -fdata-sections -mmcu=attiny2313 ./MBI5168_2313.c -o MBI5168_2313.elf
avr-objcopy -O ihex MBI5168_2313.elf MBI5168_2313.hex
avr-objdump -D -S -s MBI5168_2313.elf
avr-size MBI5168_2313.elf
