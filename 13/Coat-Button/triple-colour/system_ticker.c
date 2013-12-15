#include <avr/interrupt.h>
#include <stdint.h>
#include "hardware_conf.h"
#include "system_ticker.h"

#if !defined(NO_MOSFET) && !defined(USING_MOSFET)
#error "Something wrong with 'hardware_conf.h' !"
#endif

volatile uint16_t system_ticks = 0; // changed in an ISR

volatile uint8_t brightness_a = 0; // volatile: prevent optimizer badness
volatile uint8_t brightness_b = 0; // volatile: prevent optimizer badness

void system_ticker_setup(void)
{
	// set as output for PWM (via interrupts)
	//DDRB |= (_BV(PB3) | _BV(PB4));
	//PORTB &= ~(_BV(PB3) | _BV(PB4));
	
	// save code-space by not doing read-modify-write ops
	DDRB = (_BV(PB3) | _BV(PB4));
	PORTB = ~(_BV(PB3) | _BV(PB4));

	// using timer0
	// setting prescaler to 1: 4.8MHz system-clock --> 4.8MHz timer0-clock - ~19kHz OVF_vect clock
	//TCCR0B |= _BV(CS00);
	//TCCR0B &= ~(_BV(CS01) | _BV(CS02));
	// set to 'NORMAL' WGM0 mode.
	//TCCR0A &= ~(_BV(WGM01) | _BV(WGM00));
	//TCCR0B &= ~_BV(WGM02);

	// save code-space by not doing read-modify-write ops	
	TCCR0A = 0x00;
	TCCR0B = _BV(CS00);
	
	// enabling overflow interrupt
	TIMSK0 |= _BV(TOIE0);
}

ISR(TIM0_OVF_vect)		// on attiny2313/4313 this is named TIMER0_OVF_vect
{
	system_ticks++;

	// using dumb software PWM, as it works and gives smooth fades
	// mirrored bit angle modulation (MIBAM) has issues

	static uint8_t cycle_counter = 0;

	if (brightness_a > cycle_counter) {
#ifdef NO_MOSFET
		PORTB &= ~(_BV(PB3));	// LOW (ON)
#endif

#ifdef USING_MOSFET
		PORTB |= (_BV(PB3));	// HIGH (ON)
#endif
	} else {

#ifdef NO_MOSFET
		PORTB |= (_BV(PB3));	// HIGH (OFF)
#endif

#ifdef USING_MOSFET
		PORTB &= ~(_BV(PB3));	// LOW (OFF)
#endif
	}

	if (brightness_b > cycle_counter) {
#ifdef NO_MOSFET
		PORTB &= ~(_BV(PB4));	// LOW (ON)
#endif

#ifdef USING_MOSFET
		PORTB |= (_BV(PB4));	// HIGH (ON)
#endif
	} else {
#ifdef NO_MOSFET
		PORTB |= (_BV(PB4));	// HIGH (OFF)
#endif

#ifdef USING_MOSFET
		PORTB &= ~(_BV(PB4));	// LOW (OFF)
#endif
	}

	cycle_counter++;
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

