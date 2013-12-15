#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include "hardware_conf.h"
#include "system_ticker.h"
#include "main.h"

#if !defined(NO_MOSFET) && !defined(USING_MOSFET)
#error "Something wrong with 'hardware_conf.h' !"
#endif

//#define DEMO

uint32_t lfsr = 0xbeefcace;

#if !defined(DEMO)
uint8_t EEMEM saved_mode = 0;
#endif

#if defined(DEMO)
uint8_t mode __attribute__ ((section(".noinit")));	// provides RESET-immune storage 
#endif

int main(void)
{

#if !defined(DEMO)
	uint8_t mode = eeprom_read_byte(&saved_mode);

	setup_hw();

	if (PB0_PB2_shorted()) {	// ISP header pin #3 and #4 shorted on power-up
		delay(20000);	// requires system-ticker ISR to run!
		if (PB0_PB2_shorted()) {	// still shorted
			mode++;
			if (mode > 4) {	// cycle 0..1..2..3..4..0..1..2...
				mode = 0;
			}
			eeprom_write_byte(&saved_mode, mode);
		}
	}
#endif

#if defined(DEMO)
	setup_hw();

	wdt_enable(WDTO_8S);

	mode++;
	if (mode > 4) {		// cycle 0..1..2..3..4..0..1..2...
		mode = 0;
	}
#endif

	setup_hw();		// set and/or reset everything we need for normal operation

	while (1) {

		switch (mode) {
		case 0:
			brightness_a = 255;
			break;
		case 1:
			breathe(75, 5);	// breathe_delay, times
			break;
		case 2:
			breathe(500, 5);
			break;
		case 3:
			burst(5, 5000, 5, 500);	// bursts, burst_delay, pulses, pulse_delay
			break;
		case 4:
			flicker();
			break;
		default:
			break;
		}

	}
}

void setup_hw(void)
{
	cli();			// turn interrupts off, just in case

	DDRB = 0x00;		// set all as input
	PORTB = 0xFF;		// set all pull-ups on

	system_ticker_setup();

	sei();			// turn global irq flag on
}

void fade(uint8_t from, uint8_t to, uint16_t f_delay)
{
	int16_t counter;

	if (from <= to) {	// fade up 
		for (counter = from; counter <= to; counter++) {
			brightness_a = counter;
			delay(f_delay);
		}
	}

	if (from > to) {	// fade down 
		for (counter = from; counter >= to; counter--) {
			brightness_a = counter;
			delay(f_delay);
		}
	}
}

void breathe(uint16_t b_delay, int8_t times)
{
	while (times > 0) {
		fade(0, 255, b_delay);
		fade(255, 0, b_delay);
		times--;
	}
}

void
burst(uint8_t bursts, uint16_t burst_delay, uint8_t pulses,
      uint16_t pulse_delay)
{
	uint8_t pulse_ctr;
	uint8_t burst_ctr;

	for (burst_ctr = bursts; burst_ctr > 0; burst_ctr--) {
		for (pulse_ctr = pulses; pulse_ctr > 0; pulse_ctr--) {
			brightness_a = 255;
			delay(pulse_delay), brightness_a = 0;
			delay(pulse_delay);
		}
		delay(burst_delay);
	}
}

uint8_t PB0_PB2_shorted(void)
{
	uint8_t retval = 0;
	DDRB |= _BV(PB0);	// set PB0 as output
	PORTB &= ~_BV(PB0);	// set PB0 to LOW

	DDRB &= ~_BV(PB2);	// set PB2 as input
	PORTB |= _BV(PB2);	// pull-up on on PB2

	if (!(PINB & _BV(PB2))) {	// PB2 reads as LOW
		retval = 1;
	} else {
		retval = 0;
	}

	// don't reset pins PB0, PB2 to inputs here
	// setup_hw() will be called later in main()
	// which does all of that 

	return retval;
}

uint32_t pseudo_rand(void)
{
	// http://en.wikipedia.org/wiki/Linear_feedback_shift_register
	// Galois LFSR: taps: 32 31 29 1; characteristic polynomial: x^32 + x^31 + x^29 + x + 1 */
	lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xD0000001u);
	return lfsr;
}

void flicker(void)
{
	static uint8_t from = 0;
	static uint8_t to = 0;

	to = (uint8_t) (pseudo_rand());

	fade(from, to, (uint8_t) (pseudo_rand() >> 24));
	delay((uint8_t) (pseudo_rand() >> 24));	// random delay

	from = to;
}
