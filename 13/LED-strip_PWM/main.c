#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "config.h"
#include "util.h"
#include "system_ticker.h"
#include "adc.h"
#ifdef USE_UART
#include "uart.h"
#endif
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
		OCR0A = 254;
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

#ifndef USE_UART
#ifndef USE_BREATHE		
		adc_fade_to = 255 - adc_read(POT1);	// reverse POT1 result vs direction of rotation
		adc_delay = (uint16_t) (adc_read(POT2));
		fade(255 - OCR0A, adc_fade_to, adc_delay);	// "255 - OCR0A" --> inverted PWM
#else
		adc_fade_to = 255 - adc_read(POT1);	// reverse POT1 result vs direction of rotation
		adc_delay = (uint16_t) (adc_read(POT2));
		fade(255 - OCR0A, adc_fade_to, adc_delay);	// "255 - OCR0A" --> inverted PWM
		fade(255 - OCR0A, 0, adc_delay);	// "255 - OCR0A" --> inverted PWM
#endif
#else		
		switch (mode) {
		case 0:
			if (soft_uart_peek()) { // it seems this only works ONCE !?! WHY !!! ??? !!!
				mode = 1;
				break;
			}
			adc_fade_to = 255 - adc_read(POT1);	// reverse POT1 result vs direction of rotation
			adc_delay = (uint16_t) (adc_read(POT2));
			fade(255 - OCR0A, adc_fade_to, adc_delay);	// "255 - OCR0A" --> inverted PWM
			break;

		case 1:
			// do something useful here when serial data is received
			// the device listens on PB2 (SCK) at 9600,8,N,1
			// this pin #3 on the ISP connector (top view)
			//
			// 5   3   1
			//
			// 6   4   2
			//
			// 1: MISO
			// 2: 5V
			// 3: SCK
			// 4: MOSI
			// 5: RESET
			// 6: GND
			//
			tmp = soft_uart_read();

			//
			// A) inrease / decrease brightness by 1
			//
			if ( (tmp == '+') && (OCR0A > 0) ) {
				OCR0A--;
			}

			if ( (tmp == '-') && (OCR0A < 255) ) {
				OCR0A++;
			}

			//
			// B) use binary values to set brightness direclty
			//
			// un-comment to use - don't forget to comment-out A)
			//
			//OCR0A = tmp;

			// now back to normal
			mode = 1; // see comment above where 'soft_uart_peek()' is called
			break;

		default:
			break;
		}
#endif		
	}
}

void setup_hw(void)
{
	cli();			// turn interrupts off, just in case

	system_ticker_setup();
	adc_init();
#ifdef USE_UART
	soft_uart_setup();
#endif

	sei();			// turn global irq flag on
}

void fade(uint8_t from, uint8_t to, uint16_t f_delay)
{
	uint8_t counter = from;

	if (from <= to) {	// fade up 

		do {
			OCR0A = (255 - counter);
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
			OCR0A = (255 - counter);
			delay(f_delay);
			if (counter > 0) {
				counter--;
			} else {
				break;
			}

		} while (counter >= to);
	}
}
