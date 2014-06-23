#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "debug.h"

#ifdef DEBUG
void debug_init(void) {
	// enabling overflow interrupt
	TIMSK |= _BV(TOIE1);
}

ISR(TIMER1_OVF_vect)
{
	static volatile uint16_t counter = 0;
	static volatile uint8_t state = 0;
	counter++;

	if(counter == 140) {
		if(state == 0) {
			OCR1B++;
			if(OCR1B == 255) {
				state = 1;
			}
		} else {
			OCR1B--;
			if(OCR1B == 0) {
				state = 0;
			}
		}
		counter = 0;
	} 
}
#endif
