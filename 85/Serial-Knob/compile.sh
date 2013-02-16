#!/bin/bash

#
# relative to your HOME directory
#
ARDUINO_DIR="arduino-1.0.3/hardware/tools/avr/bin"

PATH=$PATH:$HOME/$ARDUINO_DIR

if [[ -d ./obj/Debug ]]
then
	rm -rf ./obj/Debug/*
fi

if [[ -d ./bin/Debug ]]
then
	rm -rf ./bin/Debug/*
fi

if [[ ! -d ./obj/Debug ]]
then
	mkdir -p "obj/Debug"
fi

if [[ ! -d ./bin/Debug  ]]
then
	mkdir -p "bin/Debug"
fi

avr-g++ -mmcu=attiny85 -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL      -c main.cpp -o obj/Debug/main.o

avr-g++ -mmcu=attiny85 -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL      -c system_ticker.cpp -o obj/Debug/system_ticker.o

avr-g++ -mmcu=attiny85 -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL      -c uart.cpp -o obj/Debug/uart.o

avr-g++  -o bin/Debug/source.elf obj/Debug/main.o obj/Debug/system_ticker.o obj/Debug/uart.o   -mmcu=attiny85 -Wl,-Map=bin/Debug/source.elf.map,--cref -Wl,--gc-sections,--print-gc-sections -lm -lc -v

avr-objcopy -O ihex -R .eeprom -R .eesafe bin/Debug/source.elf bin/Debug/source.elf.hex
avr-objcopy --no-change-warnings -j .eeprom --change-section-lma .eeprom=0 -O ihex bin/Debug/source.elf bin/Debug/source.elf.eep.hex
avr-objdump -h -S bin/Debug/source.elf > bin/Debug/source.elf.lss

avr-size bin/Debug/source.elf
avr-size bin/Debug/source.elf.hex

