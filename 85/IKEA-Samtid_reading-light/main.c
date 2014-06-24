#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>

#include "system_ticker.h"
#include "adc.h"
#include "pwm.h"
#include "main.h"

int main(void)
{
	setup_hw();

	initial_fade_in();

	while (1) {
		uint16_t adc_val = adc_read();
		if (adc_val > 562) {
			if (OCR1B < 255) {
				OCR1B++;
			}
		} else if (adc_val < 462) {
			if (OCR1B > 0) {
				OCR1B--;
			}
		}

		delay(25);

		if (OCR1B == 0) {
			// 2s timeout --> WDT-interrupt (ISR must be defined, else mapped to bad-interrupt --> reset)
			WDTCR = (_BV(WDIE) | _BV(WDP2) | _BV(WDP1) | _BV(WDP0));

			sleep_enable();
			sleep_cpu();
			sleep_disable();

			// using more power saving stuff (PRR register...) isn't worth it
			// current doesn't change a iota (steady 400µA).
			//
			// MCP1804: 50µA stand-by
			// PT4115:  95µA stand-by
			//
			// Something is pissing away 255µA

			pll_init();	// restart pll
			WDTCR &= ~_BV(WDIE);
		}
	}
}

void setup_hw(void)
{
	system_ticker_init();
	adc_init();
	pll_init();
	pwm_init();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sei();
}

void initial_fade_in(void)
{
	uint8_t counter;
	for (counter = 0; counter < 127; counter++) {
		OCR1B = counter;
		delay(15);
	}
}

ISR(WDT_vect, ISR_NAKED)
{
	// ISR_NAKED: no push/pop stuff --> save a few bytes, as we don't do anything here
	// just need ISR as wake-up source
	asm volatile ("reti");	// need this!
}
