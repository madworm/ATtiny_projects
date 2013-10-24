#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include "hardware_conf.h"
#include "system_ticker.h"
#include "main.h"

#if !defined(NO_MOSFET) && !defined(USING_MOSFET)
#error "Something wrong with 'hardware_conf.h' !"
#endif

uint8_t EEMEM saved_mode = 0x00;

int main(void)
{
	/*
	uint8_t mode = eeprom_read_byte(&saved_mode);

	if( PB0_PB2_shorted() ) {  // ISP header pin #3 and #4 shorted on power-up
		mode = ( mode + 1 ) % 2;
		eeprom_write_byte(&saved_mode, mode);
	}
	*/

	setup_hw(); // set and/or reset verything we need for normal operation

	/*
	switch(mode) {
		case 0:
			breathe(1000);
			break;
		case 1:
			breathe(10000);
			break;
		default:
			break;
	}
	*/

	breathe(35);

	/*
	cli();
	
	DDRB |= ( _BV(PB3) | _BV(PB4) );
	// PORTB &= ~( _BV(PB3) | _BV(PB4) );
	PORTB |= ( _BV(PB3) | _BV(PB4) );

	while(1) {}
	*/
}

void setup_hw(void)
{
	cli();			// turn interrupts off, just in case

	DDRB = 0x00; // set all as input
	PORTB = 0xFF; // set all pull-ups on
	
	system_ticker_setup();

	sei();			// turn global irq flag on
}

void fade(uint8_t from, uint8_t to, uint16_t f_delay)
{
	int16_t counter;

	if (from <= to) {	// fade up 
		for (counter = from; counter <= to; counter++) {
			OCR0A = 255 - counter;
			delay(f_delay);
		}
	}

	if (from > to) {	// fade down 
		for (counter = from; counter >= to; counter--) {
			OCR0A = 255 - counter;
			delay(f_delay);
		}
	}
}

void breathe(uint16_t b_delay) {
	while (1) {
		fade(0,255,b_delay); // from, to, delay
		fade(255,0,b_delay);
	}
}

uint8_t PB0_PB2_shorted(void) {
	uint8_t retval = 0;
	DDRB |= _BV(PB0); // set PB0 as output
	PORTB &= ~_BV(PB0); // set PB0 to LOW

	DDRB &= ~_BV(PB2); // set PB2 as input
	PORTB |= _BV(PB2); // pull-up on on PB2

	if( !(PINB & _BV(PB2)) ) { // PB2 reads as LOW
		retval = 1;
	} else {
		retval = 0;
	}

	// don't reset pins PB0, PB2 to inputs here
	// setup_hw() will be called later in main()
	// which does all of that 

	return retval;
}
