
Compiling using an installed Arduino-IDE
----------------------------------------

Only do this, if you cannot get avr-libc + avr-gcc + avr-binutils for
your distribution. You still need to install 'make'.

If you're using winblows, you can easily run some linux version
(openSUSE, kubuntu, Mint, Debian...) in a virtual machine (VirtualBox)
and use the hints below.



Variant 1: Makefile (linux)
---------------------------

After changing to the code-folder, run this to compile:

PATH=/home/YOURHOMEDIR/arduino-1.0.5/hardware/tools/avr/bin/:$PATH make

This should compile the firmware and produce 'main.hex' and 'main.eep'
and a few other files you probably won't need.

Take a look at 'flash.sh' for how to upload with avrdude.



Variant 2: IDE (linux / winblows)
---------------------------------

Install the dummy-core for the ATtiny13 chip. You will find it a few
folders on top of this directory.

Then rename all ".c" files to ".cpp" and "main.cpp" to the name of the folder
in which the code resides, e.g. "single_colour.ino". Then copy the folder
to the IDE's "sketchbook" folder (My Documents\Arduino on winblows).

Restart the IDE.



--- --- ---



If you run out of FLASH space, try using a later version of avr-gcc + avr-libc + tools.

Comparison (triple-colour):

Regular mode:
=============

avr-gcc  | avr-libc | code size (bytes)
---------------------------------------
  4.4.3	      1.7.0	1012
  4.5.1	      1.8.0	 972
  4.7.0	      1.8.0	 972


Demo mode:
==========

avr-gcc  | avr-libc | code size (bytes)
---------------------------------------
  4.4.3	      1.7.0	 940
  4.5.1	      1.8.0	 904
  4.7.0	      1.8.0	 900

