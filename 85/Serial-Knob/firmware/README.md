
Serial-Knob
===========

CODE: Rotary encoder + microcontroller. Sends commands via serial.

ATtiny25/45/85 + ALPS STEC12E08.

The microcontroller can evaluate direction of rotation, speed and acceleration + button status.
Depending on the requirements, this can be encoded in one single byte and sent to the target.

Step / direction decoding is also possible. By adding a multiplicator, this board could also
be used as a stepper-controller tester. Turn the knob, and the stepper should follow.

You will find design-files for this project in [this](https://github.com/madworm/Serial-Knob/) repository.


---

Before having PC-boards made, please make sure you know about your manufacturer's peculiarities!
Especially drill-sizes and their tolerances may vary too much and give you trouble.

