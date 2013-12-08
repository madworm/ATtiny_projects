#!/bin/bash

. ./config.txt

echo -e "\n\n-:###+++--- ---+++###:-\n"
echo -e "Compiling for target: $MCU\n"
echo -e "To change to 'attiny25' or 'attiny85' run the script like so: '$0 25' or '$0 85'\n\n"
echo -e "Press ENTER to proceed."

read

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

avr-g++ -mmcu=$MCU -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL -c main.cpp -o obj/Debug/main.o

avr-g++ -mmcu=$MCU -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL -c system_ticker.cpp -o obj/Debug/system_ticker.o

avr-g++ -mmcu=$MCU -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL -c uart.cpp -o obj/Debug/uart.o

avr-g++ -mmcu=$MCU -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL -c pwm.cpp -o obj/Debug/pwm.o

avr-g++  -o ./bin/Debug/source.elf obj/Debug/main.o obj/Debug/system_ticker.o obj/Debug/uart.o obj/Debug/pwm.o -mmcu=$MCU -Wl,-Map=./bin/Debug/source.elf.map,--cref -Wl,--gc-sections,--print-gc-sections -lm -lc -v

avr-objcopy -O ihex -R .eeprom -R .eesafe ./bin/Debug/source.elf $HEXFILE
avr-objcopy --no-change-warnings -j .eeprom --change-section-lma .eeprom=0 -O ihex ./bin/Debug/source.elf ./bin/Debug/source.elf.eep.hex
avr-objdump -h -S ./bin/Debug/source.elf > ./bin/Debug/source.elf.lss

avr-size ./bin/Debug/source.elf
avr-size $HEXFILE

