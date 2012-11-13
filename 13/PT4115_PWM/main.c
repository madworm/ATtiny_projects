#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "main.h"

int main(void)
{
 RESTART:

	setup_hw();
	delay(2000);

	initial_fade_in();

	for (;;) {
		switch (PINB & 0x06)	// only get PB2 and PB1
		{
		case 0x04:	// PB1 pulled low
			if (OCR0A < 255) {
				OCR0A++;
			}
			break;
		case 0x02:	// PB2 pulled low
			if (OCR0A > 0) {
				OCR0A--;
			}
			break;
		case 0x00:	// restart main
			goto RESTART;	// yeah I know... had to be quick and dirty ;-)
			break;
		default:
			break;
		}
		delay(5);
	}
}

void setup_hw(void)
{
	cli();			// turn interrupts off, just in case

	DDRB |= _BV(PB0);	// set as output for PWM (OC0A)
	DDRB &= ~(_BV(PB1) | _BV(PB2));	// set as input
	PORTB |= _BV(PB1) | _BV(PB2);	// turn on internal pullups
	system_ticker_setup();

	sei();			// turn global irq flag on
}

void initial_fade_in(void)
{
	uint8_t counter;
	for (counter = 0; counter < 64; counter++) {
		OCR0A = counter;
		delay(10);
	}
}
