
This requires the MIT attiny addons for the Arduino IDE.

http://hlt.media.mit.edu/?p=1695 / https://github.com/damellis/attiny/

Unzip and copy the 'attiny' subfolder to your 'sketchbook/hardware/' folder.


Linux / mac users:
==================

Furthermore, the script 'create_...sh' will patch 'Arduino.h', so that a few
functions can be overridden. A backup copy named 'Arduino.h.orig' will be
created. Use the 'undo_....sh' script to revert the changes to the Arduino-IDE. 

And as always, make sure you have good backups of important data.


Microsoft detainees:
====================

I suggest to use Linux.


If you don't wanna, this is what you've got to do by hand:

* Disable 'Ethernet' library: move it outside of Arduino-IDE's 'libraries' folder.
* Create a folder named 'Serial_Knob' in your Arduino 'sketchbook' folder.
* Copy all '.cpp' and '.hpp' files there.
* Change the extension of all '.hpp' files to '.h'
* Rename 'main.cpp' to 'Serial_Knob.ino'
* Edit all files in 'sketchbook/Serial_Knob/' and replace '.hpp' with '.h' 

* Update 'arduino-1.0.4/hardware/arduino/cores/arduino/Arduino.h' as follows:

unsigned long millis(void) __attribute__ ((weak));
unsigned long micros(void) __attribute__ ((weak));
void delay(unsigned long) __attribute ((weak));
void delayMicroseconds(unsigned int us) __attribute ((weak));

The relevant part starts at about line 105.

