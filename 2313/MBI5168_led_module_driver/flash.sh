#!/bin/bash

avrdude -c usbtiny -p attiny2313 -V -B 1 -e -U flash:w:$1:i
