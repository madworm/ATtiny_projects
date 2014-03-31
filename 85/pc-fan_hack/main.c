#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>

#include "system_ticker.h"
#include "adc.h"
#include "main.h"

#define MEASURE_INTERVAL 500

volatile uint16_t edges = 0;
volatile uint8_t flag = 0;

int main(void)
{
	setup_hw();

	uint32_t fan_timeout = 0;
	uint8_t state = 0;
	uint32_t high_time = 0;
	uint32_t low_time = 0;
	uint32_t half_period = 0;

	int32_t adc_val = 0;
	uint32_t pot_poll_timeout = 0;

	while (1) {

		if ((millis() - pot_poll_timeout) > 1000) {
			uint16_t tmp1 = adc_read();
			if (tmp1 > 512) {
				adc_val = (176 * (int32_t)(tmp1) - 80288) * ( MEASURE_INTERVAL / 10 );
			} else {
				adc_val = ( MEASURE_INTERVAL * 1000000 ) / (- 176 * (int32_t)(tmp1) + 100112 ) * 10;
			}
			pot_poll_timeout = millis();
		}

		if ((millis() - fan_timeout) > 2000) {
			flag = 1;
			edges = 2;
		}

		if (flag == 1) {
			half_period = (uint32_t) (adc_val / (edges-1));
			flag = 0;
			fan_timeout = millis();
		}

		switch (state) {
		case 0:
			if ((micros() - low_time) > half_period) {
				DDRB &= ~_BV(PB4);	// set as input (pull-up off)

				PORTB |= _BV(PB1);	// LED off

				high_time = micros();
				state = 1;
			}
			break;

		case 1:
			if ((micros() - high_time) > half_period) {
				DDRB |= _BV(PB4);	// set as output
				PORTB &= ~_BV(PB4);	// pull low

				PORTB &= ~_BV(PB1);	// LED on

				low_time = micros();
				state = 0;
			}
			break;

		default:
			break;
		}

	}

}

void setup_hw(void)
{
	DDRB &= ~_BV(PB3);	// PB3 as input (tacho in)
	PORTB |= _BV(PB3);	// pull-up on
	PCMSK |= _BV(PCINT3);	// enable pin-change interrupt
	GIMSK |= _BV(PCIE);

	DDRB |= _BV(PB4);	// PB4 as output (tacho out)
	DDRB |= _BV(PB1);	// PB1 as output (LED)

	PRR |= (_BV(PRTIM1) | _BV(PRUSI));	// save power by turning unused stuff off

	system_ticker_setup();
	adc_init();
	sei();			// turn global irq flag on, also needed as a wakeup source
}

ISR(PCINT0_vect)
{
	static uint32_t start = 0;
	static uint16_t edges_ctr = 0;

	edges_ctr++;

	if ((millis() - start) > MEASURE_INTERVAL) {
		edges = edges_ctr;
		flag = 1;
		edges_ctr = 0;
		start = millis();
	}
}
