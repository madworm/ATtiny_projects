#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "cplex.h"
#include "main.h"

int main(void)
{

	setup_hw();

	uint16_t led_delay = 10000;

	if (PB0_PB2_shorted()) {	// ISP header pin #3 and #4 shorted on power-up
		delay(20000);	// requires system-ticker ISR to run!
		if (PB0_PB2_shorted()) {	// still shorted
			led_delay=2000;
		}
	}

	setup_hw();		// set and/or reset everything we need for normal operation

	while (1) {

		static uint8_t index = 0;

		ALL_OFF;
		DDRB = DDRB_array[index];
		PORTB = PORTB_array[index];
		delay(led_delay);
		index++;
		if ( index == 8 ) {
			index = 0;
		}

	}
}

void setup_hw(void)
{
	cli();			// turn interrupts off, just in case

	DDRB = 0x00;		// set all as input
	PORTB = 0x00;		// set all pull-ups off

	system_ticker_setup();

	sei();			// turn global irq flag on
}

uint8_t PB0_PB2_shorted(void)
{
	uint8_t retval = 0;

	DDRB |= _BV(PB0);	// set PB0 as output
	PORTB &= ~_BV(PB0);	// set PB0 to LOW

	DDRB &= ~_BV(PB2);	// set PB2 as input
	PORTB |= _BV(PB2);	// pull-up on on PB2

	delay(20000);

	if (!(PINB & _BV(PB2))) {	// PB2 reads as LOW
		retval = 1;
	} else {
		retval = 0;
	}

	DDRB = 0x00;
	PORTB = 0x00;

	return retval;
}
