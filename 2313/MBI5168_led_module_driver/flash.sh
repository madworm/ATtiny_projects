#!/bin/bash

avrdude -c usbtiny -p attiny2313 -e -U flash:w:$1:i
