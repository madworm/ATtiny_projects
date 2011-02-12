#!/bin/bash

avr-gcc -Os -g -fno-exceptions -ffunction-sections -fdata-sections -mmcu=attiny2313 ./source.c -o source.elf
avr-objcopy -O ihex source.elf source.hex
avr-objdump -D -S -s source.elf
avr-size source.elf
