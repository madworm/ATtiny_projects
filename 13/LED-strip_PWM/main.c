#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "system_ticker.h"
#include "adc.h"
#include "uart.h"
#include "main.h"

int main(void)
{
	uint8_t adc_fade_to;
	uint16_t adc_delay;
	uint8_t mode = 0;
	uint8_t tmp = 0;

	setup_hw();

	/*
	while (1) {
		OCR0A = 0;
		delay_ms(500);
		OCR0A = 255;
		delay_ms(500);
	}
	*/

	/*
	delay_ms(2500);

	while(1) {
		OCR0A = 0;
		delay_ms(250);
		OCR0A = 255;
		soft_uart_send('+',++tmp); // 21 worked for 9600
		delay_ms(250);
	}
	*/

	while (1) {

		//if(soft_uart_peek()) {
		//	pulse_PB0_ms(250);
		//}

		switch (mode) {
		case 0:
			if (soft_uart_peek()) {
				mode = 1;
				break;
			}
			adc_fade_to = 255 - adc_read(POT1);	// reverse POT1 result vs direction of rotation
			adc_delay = (uint16_t) (adc_read(POT2));
			fade(255 - OCR0A, adc_fade_to, adc_delay);	// "255 - OCR0A" --> inverted PWM
			break;

		case 1:
			// do something useful here when serial data is received
			tmp = soft_uart_read();

			if ( (tmp == '+') && (OCR0A > 0) ) {
				OCR0A--;
			}

			if ( (tmp == '-') && (OCR0A < 255) ) {
				OCR0A++;
			}
			// now back to normal
			mode = 1;
			break;

		default:
			break;
		}
	}
}

void setup_hw(void)
{
	cli();			// turn interrupts off, just in case

	system_ticker_setup();
	adc_init();
	soft_uart_setup();

	sei();			// turn global irq flag on
}

void fade(uint8_t from, uint8_t to, uint16_t f_delay)
{
	int16_t counter;

	if (from <= to) {	// fade up
		for (counter = from; counter <= to; counter++) {
			OCR0A = (255 - counter);
			delay(f_delay);
		}
	}

	if (from > to) {	// fade down
		for (counter = from; counter >= to; counter--) {
			OCR0A = (255 - counter);
			delay(f_delay);
		}
	}
}
