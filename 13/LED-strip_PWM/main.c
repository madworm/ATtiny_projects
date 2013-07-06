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
	uint8_t adc_val = 0;

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
		adc_val = adc_read(POT2);
		OCR0A = adc_val;
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
