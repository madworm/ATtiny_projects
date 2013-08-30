#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "util.h"

void delay_ms(uint16_t ms)
{
	while(ms) {
		_delay_ms(1);
		ms--;
	}
}

void delay_us(uint16_t us)
{
	while(us) {
		_delay_us(1);
		us--;
	}
}

void pulse_PB0_ms(uint16_t ms)
{
	OCR0A = 254; // off
	delay_ms(2*ms);
	OCR0A = 0; // on
	delay_ms(ms);
	OCR0A = 254; // off
	delay_ms(2*ms);
}

void pulse_PB1_us(uint16_t us)
{
	DDRB |= _BV(PB1); // as output
	PORTB &= ~_BV(PB1); // LOW
	delay_us(2*us);
	PORTB |= _BV(PB1); // HIGH
	delay_us(us);
	PORTB &= ~_BV(PB1); // LOW
	delay_us(2*us);
	DDRB &= ~_BV(PB1);; // as input
}
