#!/bin/bash

. ../config.txt

FOLDER=Serial_Knob
ARDUINO_H=$HOME/$ARDUINO_DIR/hardware/arduino/cores/arduino/Arduino.h

echo -e "\n\n  You will need this: http://hlt.media.mit.edu/?p=1695 / https://github.com/damellis/attiny/\n"

if [[ ! -d ./$FOLDER ]]
then
	mkdir $FOLDER
fi

cp ../*.cpp ./$FOLDER
cp ../*.hpp ./$FOLDER
rename hpp h ./$FOLDER/*.hpp
mv ./$FOLDER/main.cpp ./$FOLDER/Serial_Knob.ino

for PATCH in `ls *.patch`
do
	patch ./$FOLDER/${PATCH/%.patch/} $PATCH
done

echo -e "\n  About to patch '$ARDUINO_H'\n"
echo -e "  This should not have any negative effects. Just in case, there is a backup!\n"

if [[ -e $ARDUINO_H.orig ]]
then
	echo -e "    Patch was already applied.\n"
else
	patch -b $ARDUINO_H Arduino.h__Serial-Knob.diff
	echo -e "\n    Done!\n"
fi

if [[ -e $HOME/$ARDUINO_DIR/libraries/Ethernet ]]
then
	echo -e "  Disabling the 'Ethernet' library\n"
	mv -b $HOME/$ARDUINO_DIR/libraries/Ethernet $HOME/$ARDUINO_DIR/libraries/Ethernet__off__by_Serial-Knob	
fi

echo -e "  Move the folder '$FOLDER' to your Arduino 'sketchbook' folder.\n"

