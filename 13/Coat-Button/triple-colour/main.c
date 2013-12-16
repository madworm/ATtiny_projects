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

uint16_t lfsr = 0xbeef;

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

	if (PB0_PB2_shorted()) {	// ISP header pin #3 and #4 shorted on power-up
		mode++;
		if (mode > 10) {	// cycle 0..1..2..3..4..5..6..7..8..9..10..0..1..2...
			mode = 0;
		}
		eeprom_write_byte(&saved_mode, mode);
	}
#endif

#if defined(DEMO)
	wdt_enable(WDTO_8S);

	mode++;
	if (mode > 10) {	// cycle 0..1..2..3..4..5..6..7..8..9..10..0..1..2...
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
			brightness_b = 255;
			break;
		case 2:
			brightness_a = 255;
			brightness_b = 255;
			break;
		case 3:
			breathe(75, 0, 5);	// delay, color, times
			break;
		case 4:
			breathe(75, 1, 5);
			break;
		case 5:
			breathe(75, 2, 5);
			break;
		case 6:
			burst(5, 5000, 5, 500, 0);
			break;
		case 7:
			burst(5, 5000, 5, 500, 1);
			break;
		case 8:
			burst(5, 5000, 5, 500, 2);
			break;
		case 9:
			rainbow(100);
			break;
		case 10:
			random_color(100);
			break;
		default:
			break;
		}

	}
}

void setup_hw(void)
{
	DDRB = 0x00;		// set all as input
	PORTB = 0xFF;		// set all pull-ups on

	system_ticker_setup();

	sei();			// turn global irq flag on
}

void fade(uint8_t from, uint8_t to, uint16_t f_delay, uint8_t color)
{
	uint8_t counter = from;

	if (from <= to) {	// fade up 

		do {
			helper(color, counter);	// save space by avoiding code duplication
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
			helper(color, counter);
			delay(f_delay);
			if (counter > 0) {
				counter--;
			} else {
				break;
			}

		} while (counter >= to);
	}
}

void helper(uint8_t color, uint8_t value)
{
	if (color == 0) {
		brightness_a = value;
	}
	if (color == 1) {
		brightness_b = value;
	}
	if (color == 2) {
		brightness_a = value;
		brightness_b = value;
	}
}

void breathe(uint16_t b_delay, uint8_t color, uint8_t times)
{
	do {
		fade(0, 255, b_delay, color);
		fade(255, 0, b_delay, color);

		if (times > 0) {
			times--;
		} else {
			break;
		}
	} while (times >= 0);
}

void burst(uint8_t bursts, uint16_t burst_delay, uint8_t pulses,
	   uint16_t pulse_delay, uint8_t color)
{
	uint8_t pulse_ctr;
	uint8_t burst_ctr;

	for (burst_ctr = bursts; burst_ctr > 0; burst_ctr--) {
		for (pulse_ctr = pulses; pulse_ctr > 0; pulse_ctr--) {
			if (color == 0) {
				brightness_a = 255;
				delay(pulse_delay);
				brightness_a = 0;
				delay(pulse_delay);
			}
			if (color == 1) {
				brightness_b = 255;
				delay(pulse_delay);
				brightness_b = 0;
				delay(pulse_delay);
			}
			if (color == 2) {
				brightness_a = 255;
				brightness_b = 255;
				delay(pulse_delay);
				brightness_a = 0;
				brightness_b = 0;
				delay(pulse_delay);
			}
		}
		delay(burst_delay);
	}
}

void rainbow(uint16_t rainbow_delay)
{
	uint8_t ctr;

	ctr = 0;

	do {
		brightness_a = ctr;	// color a UP
		brightness_b = 255 - ctr;	// color b DOWN
		delay(rainbow_delay);

		if (ctr < 255) {
			ctr++;
		} else {
			break;
		}
	} while (ctr <= 255);

	ctr = 0;

	do {
		brightness_b = ctr;	// color b UP
		delay(rainbow_delay);

		if (ctr < 255) {
			ctr++;
		} else {
			break;
		}
	} while (ctr <= 255);

	ctr = 255;

	do {
		brightness_a = ctr;	// color a DOWN
		delay(rainbow_delay);

		if (ctr > 0) {
			ctr--;
		} else {
			break;
		}
	} while (ctr >= 0);
}

uint8_t PB0_PB2_shorted(void)
{
	uint8_t retval = 0;
	//DDRB |= _BV(PB0);     // set PB0 as output
	//PORTB &= ~_BV(PB0);   // set PB0 to LOW

	//DDRB &= ~_BV(PB2);    // set PB2 as input
	//PORTB |= _BV(PB2);    // pull-up on on PB2

	// equivalent of the above read-modify-write ops
	DDRB = _BV(PB0);
	PORTB = _BV(PB2);

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

uint16_t pseudo_rand(void)
{
	// http://en.wikipedia.org/wiki/Linear_feedback_shift_register
	// Galois LFSR: taps: 16 15 14 1; characteristic polynomial: x^16 + x^15 + x^14 + x + 1 */
	lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xD001u);
	return lfsr;
}

void random_color(uint16_t fade_delay)
{
	uint8_t from_a = brightness_a;
	uint8_t from_b = brightness_b;

	uint8_t to_a = (uint8_t) (pseudo_rand() >> 8);
	uint8_t to_b = (uint8_t) (pseudo_rand() >> 8);

	fade(from_a, to_a, fade_delay, 0);
	fade(from_b, to_b, fade_delay, 1);
}
