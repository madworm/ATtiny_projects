#include <avr/interrupt.h>
#include <stdint.h>
#include "system_ticker.h"

volatile uint16_t system_ticks = 0;	// changed in an ISR

void system_ticker_setup(void)
{
	// using timer0
	// setting prescaler to 1: 4.8MHz system-clock --> 4.8MHz timer0-clock - ~19kHz OVF_vect clock
	TCCR0B |= _BV(CS00);
	TCCR0B &= ~(_BV(CS01) | _BV(CS02));

	// NO PWM
	TCCR0A = 0x00;

	// enabling overflow interrupt
	TIMSK0 |= _BV(TOIE0);
}

ISR(TIM0_OVF_vect)		// on attiny2313/4313 this is named TIMER0_OVF_vect
{
	system_ticks++;
}

uint16_t time(void)
{
	uint8_t _sreg = SREG;
	uint16_t time;
	cli();
	time = system_ticks;
	SREG = _sreg;
	return time;
}

void delay(uint16_t ticks)
{
	uint16_t start_time = time();
	while ((time() - start_time) < ticks) {
		// just wait here
	}
}
