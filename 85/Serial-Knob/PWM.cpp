#include <avr/io.h>
#include <stdint.h>

#include "PWM.hpp"

void setup_PWM(void) {
	PORTB &= ~_BV(PB4); //pull-up off
	DDRB |= _BV(PB4); // set as output
	// turn power for timer1 back on - we want to use it!
	PRR &= ~_BV(PRTIM1);
	// set prescaler of timer1 to 1 and TOP to 255 --> ~ 30kHz PWM
	TCCR1 |= _BV(CS10); // default prescaler bits: all 0
	OCR1C = 255; // set TOP for tcnt1
	// enable PWM for OC1B output, clear OC1B on compare match
	GTCCR |= _BV(PWM1B) | _BV(COM1B1);
	OCR1B = 0; // start off
}
