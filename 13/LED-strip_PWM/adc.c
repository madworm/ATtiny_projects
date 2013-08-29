#include <avr/io.h>
#include <stdint.h>
#include "adc.h"

uint8_t adc_read(uint8_t channel)
{
	ADMUX &= ~(_BV(MUX1) | _BV(MUX0));	// reset to channel PB5

	ADMUX |= channel;	// valid values for this board: 0 (PB5) - 1 (PB2) - 2 (PB4) - 3 (PB3)
	ADCSRA |= _BV(ADSC);	// start conversion
	while (ADCSRA & _BV(ADSC)) {
		// wait until ADC is done
	}
	return ADCH;
}

void adc_init(void)
{
  	DDRB &= ~(_BV(PB3) | _BV(PB4));	// set as input (POT1, POT2)
	ADCSRA |= _BV(ADEN);	// enable ADC
	ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));	// set prescaler to 128 for stable readings
	ADMUX |= _BV(ADLAR);	// set to left-aligned. we only need 8-bit and read ADCH only
	ADMUX &= ~(_BV(REFS0));	// VREF = VCC
}
