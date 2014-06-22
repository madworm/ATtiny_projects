#include <avr/io.h>
#include <stdint.h>

#include "pwm.h"

void pwm_init(void) {
  
  	PORTB &= ~_BV(PB4); //pull-up off
	DDRB |= _BV(PB4); // set as output
  
    	// timer1
	PLLCSR |= _BV(PLLE); // enable PLL
	//_delay_ms(1); // datasheet: wait 100µs for PLL to stabilize PLOCK-bit useless in that interval
	while( !(PLLCSR & _BV(PLOCK)) ) {
		// wait until PLOCK-bit is set
	}
	PLLCSR |= _BV(PCKE); // set PLL as clock for timer1
  
	// set prescaler of timer1 to 64 and TOP to 255 --> ~ 4kHz PWM
	// system clock (8MHz) * 8 (PLL) / 64 / 255
      	// for some reason a higher PWM frequency creates a beating in brightness
	// when fading up or down. need to check PWM with scope
	TCCR1 |= (_BV(CS12) | _BV(CS11) | _BV(CS10)); 
	OCR1C = 255; // set TOP for TCNT1
	
	// enable PWM for OC1B output, clear OC1B on compare match
	GTCCR |= (_BV(PWM1B) | _BV(COM1B1));
	OCR1B = 0; // start off
}
