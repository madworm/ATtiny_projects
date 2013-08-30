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
	OCR0A = 255; // off
	delay_ms(2*ms);
	OCR0A = 0; // on
	delay_ms(ms);
	OCR0A = 255; // off
	delay_ms(2*ms);
}

void pulse_PB0_us(uint16_t us)
{
	OCR0A = 255; // off
	delay_us(2*us);
	OCR0A = 0; // on
	delay_us(us);
	OCR0A = 255; // off
	delay_us(2*us);
}
