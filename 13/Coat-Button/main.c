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
	setup_hw();

	uint8_t mode = eeprom_read_byte(&saved_mode);

	if( 1 ) {  // ISP header pin #3 and #4 shorted on power-up
		mode = ( mode + 1 ) % 2;
		eeprom_write_byte(&saved_mode, mode);
	}

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
