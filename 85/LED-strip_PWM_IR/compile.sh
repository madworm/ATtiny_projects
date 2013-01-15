#!/bin/bash

avr-g++ -mmcu=attiny85 -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL      -c IR_receiver.cpp -o obj/Debug/IR_receiver.o

avr-g++ -mmcu=attiny85 -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL      -c main.cpp -o obj/Debug/main.o

avr-g++ -mmcu=attiny85 -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL      -c system_ticker.cpp -o obj/Debug/system_ticker.o

avr-g++ -mmcu=attiny85 -fexpensive-optimizations -Os -Wmain -Wall -g -gdwarf-2 -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-exceptions -DF_CPU=8000000UL      -c uart.cpp -o obj/Debug/uart.o

avr-g++  -o bin/Debug/source.elf obj/Debug/IR_receiver.o obj/Debug/main.o obj/Debug/system_ticker.o obj/Debug/uart.o   -mmcu=attiny85 -Wl,-Map=bin/Debug/source.elf.map,--cref -Wl,--gc-sections,--print-gc-sections  /usr/local/avr/avr/lib/libc.a /usr/local/avr/avr/lib/libm.a 

avr-objcopy -O ihex -R .eeprom -R .eesafe bin/Debug/source.elf bin/Debug/source.elf.hex
avr-objcopy --no-change-warnings -j .eeprom --change-section-lma .eeprom=0 -O ihex bin/Debug/source.elf bin/Debug/source.elf.eep.hex
avr-objdump -h -S bin/Debug/source.elf > bin/Debug/source.elf.lss

avr-size bin/Debug/source.elf
avr-size bin/Debug/source.elf.hex

