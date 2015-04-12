#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "config.h"
#include "util.h"
#include "system_ticker.h"
#include "adc.h"
#include "main.h"

int main(void)
{
	uint8_t adc_fade_to;
	uint16_t adc_delay;

	setup_hw();

	while (1) {
		adc_fade_to = 255 - adc_read(POT1);	// reverse POT1 result vs direction of rotation
		adc_delay = (uint16_t) (adc_read(POT2));
		fade(255 - OCR0A, adc_fade_to, adc_delay); // "255 - OCR0A": current brightness (inverted PWM)
		fade(255 - OCR0A, 2, adc_delay); // fade back to 2 for "breathing"
	}
}

void setup_hw(void)
{
	cli();			// turn interrupts off, just in case

	system_ticker_setup();
	adc_init();

	sei();			// turn global irq flag on
}

void fade(uint8_t from, uint8_t to, uint16_t f_delay)
{
	uint8_t counter = from;

	if (from <= to) {	// fade up 

		do {
			OCR0A = (255 - counter);
			delay(f_delay);
			if (counter < 255) {
				counter++;
			} else {
				break;
			}
		} while (counter <= to);

	}

	if (from > to) {	// fade down 

		do {
			OCR0A = (255 - counter);
			delay(f_delay);
			if (counter > 0) {
				counter--;
			} else {
				break;
			}

		} while (counter >= to);
	}
}
