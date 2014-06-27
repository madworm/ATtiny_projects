
* Should work with ATtiny25/45/85 (adjust 'Makefile').

* When turned on, it starts at minimal brightness and stays there for 3 seconds.
  This allows your eyes to adapt to the light a bit. Then it fades up to 50%.

* Pressing the up/down buttons changes brightness.

* When the lowest brightness level is reached, fading stops for 1.5 seconds
  (from either direction). This makes it very easy to set the lamp to that level,
  which is especially nice at night. It is very easy on night-adapted eyes.

* Continuously pressing the 'down' button until the lamp goes off will put it
  into sleep mode (just 400µA + power-supply losses).

* To wake the lamp up again, press the 'up' button for a couple of seconds.

* To 'reset' the lamp, remove power for some time. Depending on the type
  of power-supply you use, this may take anywhere from 0 to 30 seconds or
  thereabouts. Laptop power-supplies tend to store quite a lot of energy.

