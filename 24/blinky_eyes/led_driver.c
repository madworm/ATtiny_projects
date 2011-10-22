#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "led_driver.h"
#include "spi.h"
#include "system_ticker.h"

uint8_t brightness_red;     /* memory for RED LEDs */
uint8_t brightness_green;	/* memory for GREEN LEDs */
uint8_t brightness_blue;	/* memory for BLUE LEDs */

void led_driver_setup(void)
{
    // set prescaler to 256
    TCCR1B |= (_BV(CS12));
    TCCR1B &= ~(_BV(CS11) | _BV(CS10));
    // set WGM mode 4: CTC using OCR1A
    TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));
    TCCR1B |= _BV(WGM12);
    TCCR1B &= ~_BV(WGM13);
    // normal operation - disconnect PWM pins
    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));
    // set top value for TCNT1
    OCR1A = __TRUE_RGB_OCR1A;
    // enable COMPA isr
    TIMSK1 |= _BV(OCIE1A);
}

/*
 * PWM_BLOCK_START: all functions in this block are related to PWM mode !
 */

/*
 * other functions
 */

void fader(void)
{				/* fade the matrix form BLACK to WHITE and back */
	uint8_t ctr1;

	for (ctr1 = 0; ctr1 <= __max_brightness; ctr1++) {
		set_led_rgb(ctr1, ctr1, ctr1);
		delay(__fade_delay);
	}

	for (ctr1 = __max_brightness; (ctr1 >= 0) & (ctr1 != 255); ctr1--) {
		set_led_rgb(ctr1, ctr1, ctr1);
		delay(__fade_delay);
	}
}

void fader_hue(void)
{				/* cycle the color of the whole matrix */
	uint16_t ctr1;
	for (ctr1 = 0; ctr1 < 360; ctr1 = ctr1 + 3) {
		set_led_hsv(ctr1, 255, 255);
		delay(__fade_delay);
	}
}

/*
 *basic functions to set the LEDs
 */

void set_led_red(uint8_t red)
{
	brightness_red = red;
}

void set_led_green(uint8_t green)
{
	brightness_green = green;
}

void set_led_blue(uint8_t blue)
{
	brightness_blue = blue;
}

void set_led_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
	set_led_red(red);
	set_led_green(green);
	set_led_blue(blue);
}

void set_led_hsv(uint16_t hue, uint8_t sat, uint8_t val)
{

	/* BETA */

	/* finally thrown out all of the float stuff and replaced with uint16_t
	 *
	 * hue: 0-->360 (hue, color)
	 * sat: 0-->255 (saturation)
	 * val: 0-->255 (value, brightness)
	 *
	 */

	hue = hue % 360;
	uint8_t sector = hue / 60;
	uint8_t rel_pos = hue - (sector * 60);
	uint16_t const mmd = 255UL * 255UL;	/* maximum modulation depth */
	uint16_t top = val * 255;
	uint16_t bottom = val * (255 - sat);	/* (val*255) - (val*255)*(sat/255) */
	uint16_t slope = (uint16_t) (val) * (uint16_t) (sat) / 120;	/* dy/dx = (top-bottom)/(2*60) -- val*sat: modulation_depth dy */
	uint16_t a = bottom + slope * rel_pos;
	uint16_t b =
	    bottom + (uint16_t) (val) * (uint16_t) (sat) / 2 + slope * rel_pos;
	uint16_t c = top - slope * rel_pos;
	uint16_t d =
	    top - (uint16_t) (val) * (uint16_t) (sat) / 2 - slope * rel_pos;

	uint16_t R, G, B;

	if (sector == 0) {
		R = c;
		G = a;
		B = bottom;
	} else if (sector == 1) {
		R = d;
		G = b;
		B = bottom;
	} else if (sector == 2) {
		R = bottom;
		G = c;
		B = a;
	} else if (sector == 3) {
		R = bottom;
		G = d;
		B = b;
	} else if (sector == 4) {
		R = a;
		G = bottom;
		B = c;
	} else {
		R = b;
		G = bottom;
		B = d;
	}

	uint16_t scale_factor = mmd / __max_brightness;

	R = (uint8_t) (R / scale_factor);
	G = (uint8_t) (G / scale_factor);
	B = (uint8_t) (B / scale_factor);

	set_led_rgb(R, G, B);
}

/*
 * Functions dealing with hardware specific jobs / settings
 */

ISR(TIM1_COMPA_vect) /* stupid brute force PWM */
{
    DISPLAY_ON;
    uint8_t tmp;
	uint8_t cycle;
		for (cycle = 0; cycle <= (__max_brightness -1); cycle++) {
                tmp = 0;
				if (cycle < brightness_red) {
					tmp |= _BV(2);
				}
				if (cycle < brightness_green) {
					tmp |= _BV(1);
				}
				if (cycle < brightness_blue) {
					tmp |= _BV(0);
				}
			    spi_transfer(tmp);
                LATCH;
		}
    DISPLAY_OFF;
}

/*
 * PWM_BLOCK_END: all functions in this block are related to PWM mode !
 */
