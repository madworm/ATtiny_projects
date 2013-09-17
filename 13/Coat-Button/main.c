#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "main.h"

int main(void)
{
	setup_hw();
	delay(16000);

	while (1) {
		fade(0,255,10); // from, to, delay
		fade(255,0,10);
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
