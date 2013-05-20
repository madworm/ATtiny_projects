#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include <util/delay.h>

#include "system_ticker.hpp"
#include "uart.hpp"
#include "IR_receiver.hpp"
#include "main.hpp"

#define FADE_DELAY 100

int main(void)
{
	setup_hw();
	IR_code_t IR_code = MISMATCH;

	while (1) {
		static uint8_t fade_up_running = 0;
		static uint8_t fade_down_running = 0;
		static uint32_t last_change = 0;

		if (IR_available()) {
			IR_code = eval_IR_code();
		}

		if (IR_code == VOL_UP) {
			fade_up_running = 0;
			fade_down_running = 0;

			if (OCR0A > 0) {
				OCR0A--;
			} else {
				OCR0A = 0;
			}
		}

		if (IR_code == VOL_DOWN) {
			fade_up_running = 0;
			fade_down_running = 0;

			if (OCR0A < 255) {
				OCR0A++;
			} else {
				OCR0A = 255;
			}
		}

		if ((IR_code == ARROW_UP) || (fade_up_running == 1)) {
			if ((IR_code == ARROW_UP) && (fade_up_running == 1)) {
				fade_up_running = 0; // stop fade-up when ARROW_UP is pressed while fade-up is running
			} else {
				fade_up_running = 1;
				fade_down_running = 0;
				if ((millis() - last_change) > FADE_DELAY) { // when it is time for the next step, do it
					if (OCR0A > 0) {
						OCR0A--;
					} else {
						OCR0A = 0; // inverted PWM
						fade_up_running = 0; // all done, nothing more to fade
					}
					last_change = millis(); // remember when last step was done
				}
			}
		}

		if ((IR_code == ARROW_DOWN) || (fade_down_running == 1)) {
			if ((IR_code == ARROW_DOWN) && (fade_down_running == 1)) {
				fade_down_running = 0; // stop fade-down when ARROW_DOWN is pressed while fade-down is running
			} else {
				fade_down_running = 1;
				fade_up_running = 0;
				if ((millis() - last_change) > FADE_DELAY) { // when it is time for the next step, do it
					if (OCR0A < 255) {
						OCR0A++;
					} else {
						OCR0A = 255; // inverted PWM
						fade_down_running = 0; // all done, nothing more to fade
					}
					last_change = millis(); // remember when last step was done
				}
			}
		}
		IR_code = MISMATCH; // clear IR_code, so we don't re-interpret old data
	}
	return 0;
}

void setup_hw(void)
{
	/*
	 * save some power
	 *
	 */

	DDRB = 0x00;		// all inputs
	PORTB = 0xFF;		// all pull-ups on
	ACSR |= _BV(ACD);	// disable the analog comparator, ACIE is already zero (default value)
	PRR |= _BV(PRTIM1) | _BV(PRUSI) | _BV(PRADC);	// turn off the clock for TIMER1, USI and ADC

	// configure what we need to use
	PORTB &= ~_BV(PB0);	// pull-up on PWM pin off
	DDRB |= _BV(PB0);	// PWM output pin

	system_ticker_setup();
	sei();			// turn global irq flag on, also needed as a wakeup source
	init_IR();
	soft_uart_init();
}
