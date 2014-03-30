#include <avr/io.h>
#include <stdint.h>
#include "adc.h"

uint16_t adc_read(void)
{
	ADMUX |= _BV(MUX0);	// reset to channel PB2

	ADCSRA |= _BV(ADSC);	// start conversion
	while (ADCSRA & _BV(ADSC)) {
		// wait until ADC is done
	}
	return ADC;
}

void adc_init(void)
{
	DDRB &= ~_BV(PB2);	// set as input
	ADCSRA |= _BV(ADEN);	// enable ADC
	ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));	// set prescaler to 128 for stable readings
	ADMUX &= ~(_BV(REFS1) | _BV(REFS0));	// VREF = VCC
}
