#include <avr/interrupt.h>
#include <stdint.h>
#include "system_ticker.h"

volatile uint16_t system_ticks = 0;

void system_ticker_setup(void)
{
	// set as output for PWM
	PORTB &= _BV(PB0); // pull-up OFF
	DDRB |= _BV(PB0); // set as output
	OCR0A = 255;
	// using timer0
	// setting prescaler to 8: 9.6MHz system-clock --> 1.2MHz timer0-clock - ~5kHz PWM clock
	TCCR0B |= _BV(CS01);
	TCCR0B &= ~(_BV(CS02) | _BV(CS00));
	// set to 'NORMAL' WGM0 mode. see comments in 'uart.c' for reason
	TCCR0A &= ~(_BV(WGM01) | _BV(WGM00));
	TCCR0B &= ~_BV(WGM02);
	// no PWM on PB0 (OC0A) 
	TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0));
	// enabling overflow interrupt
	TIMSK0 |= _BV(TOIE0);
	// enable compare match interrupt A
	TIMSK0 |= _BV(OCIE0A);
}

ISR(TIM0_COMPA_vect)
{
	// manual PWM...
	
	//
	// due to interrupt latency, runtime...
	// setting the pin at 255 and turning it off again in the overflow ISR
	// doesn't producte 0% duty cycle.
	//
	if(OCR0A < 254) {
		PORTB |= _BV(PB0); // HIGH
	}
}

ISR(TIM0_OVF_vect)		// on attiny2313/4313 this is named TIMER0_OVF_vect
{
	system_ticks++;
	
	// manual PWM - reason: need OCR0B updated IMMEDIATELY for soft-uart
	PORTB &= ~_BV(PB0); // LOW
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
