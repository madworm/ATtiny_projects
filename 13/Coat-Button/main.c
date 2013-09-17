#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "main.h"

uint8_t EEMEM saved_mode = 0x00;

int main(void)
{
	uint8_t mode = eeprom_read_byte(&saved_mode);

	if( PB3_PB4_shorted() ) {  // ISP header pin #3 and #4 shorted on power-up
		mode = ( mode + 1 ) % 2;
		eeprom_write_byte(&saved_mode, mode);
	}

	setup_hw(); // set everything we need for normal operation

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
}

void setup_hw(void)
{
	cli();			// turn interrupts off, just in case

	DDRB = 0x00; // set all as input
	PORTB = 0xFF; // set all pull-ups on
	DDRB |= ( _BV(PB3) | _BV(PB4) ); // PB3 and PB4 as outputs for LEDs

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

uint8_t PB3_PB4_shorted(void) {
	uint8_t retval = 0;
	DDRB |= _BV(PB3); // set PB3 as output
	PORTB &= ~_BV(PB3); // set PB3 to LOW

	DDRB &= ~_BV(PB4); // set PB4 as input
	PORTB |= _BV(PB4); // pull-up on on PB4

	if( !(PINB & _BV(PB4)) ) { // PB4 reads as LOW
		retval = 1;
	} else {
		retval = 0;
	}

	return retval;
}
