
This requires the MIT attiny addons for the Arduino IDE.

http://hlt.media.mit.edu/?p=1695 / https://github.com/damellis/attiny/

Unzip and copy the 'attiny' subfolder to your 'sketchbook/hardware/' folder.


Linux / mac users:
==================

Furthermore, the script 'create_...sh' will patch 'Arduino.h', so that a few functions can be overridden.
A backup copy named 'Arduino.h.orig' will be created. Use the 'undo_....sh' script to revert the changes
to the Arduino-IDE. 

And as always, make sure you have good backups of important data.


Microsoft detainees:
====================

I suggest to use Linux.


If you don't wanna, this is what you've got to do by hand:

* Disable the 'Ethernet' library by moving its folder outside of Arduino-IDE's 'libraries' subfolder.
* Copy the 'Serial_Knob' folder to your 'sketchbook' folder.
* Remove everything in that folder - except .hpp and .cpp files.
* Change the extension of all .hpp files to .h
* Edit all files with a text-editor and replace '.hpp' with '.h' wherever you find it.

* Update 'arduino-1.0.4/hardware/arduino/cores/arduino/Arduino.h' as follows:

unsigned long millis(void) __attribute__ ((weak));
unsigned long micros(void) __attribute__ ((weak));
void delay(unsigned long) __attribute ((weak));
void delayMicroseconds(unsigned int us) __attribute ((weak));

The relevant part starts at about line 105.

