#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "adc.h"
#include "main.h"

int main(void)
{
	//uint8_t mode = eeprom_read_byte(0);
	uint8_t adc_fade_to;
	uint16_t adc_delay;

	setup_hw();

	/*
	 * DEBUG PWM blinker
	 *
	 while (1) {
	 OCR0A = 0;
	 delay(10000);
	 OCR0A = 255;
	 delay(10000);
	 }
	 *
	 */

	while (1) {
		adc_fade_to = 255 - adc_read(POT1); // reverse POT1 result vs direction of rotation
		adc_delay = (uint16_t)(adc_read(POT2));
		fade(255 - OCR0A, adc_fade_to, 4 * adc_delay); // "255 - OCR0A" --> inverted PWM
	}
}

void setup_hw(void)
{
	cli();			// turn interrupts off, just in case

	DDRB |= _BV(PB0);	// set as output for PWM (OC0A)
	DDRB &= ~(_BV(PB3) | _BV(PB4));	// set as input (POT1, POT2)
	system_ticker_setup();
	adc_init();

	sei();			// turn global irq flag on
}

void fade(uint8_t from, uint8_t to, uint16_t f_delay)
{
	int16_t counter;

	if (from <= to) {	// fade up
		for (counter = from; counter <= to; counter++) {
			OCR0A = (255 - counter);
			delay(f_delay);
		}
	}

	if (from > to) {	// fade down
		for (counter = from; counter >= to; counter--) {
			OCR0A = (255 - counter);
			delay(f_delay);
		}
	}
}
