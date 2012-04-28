#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "led_driver.h"
#include "system_ticker.h"
#include "status_leds.h"
#include "spi.h"

int16_t lamp_brightness = 0; // variable is mapped down to individual channels
uint8_t led_brightness[8] = {0,0,0,0,0,0,0,0};

static void set_led_pattern(void);

void led_driver_setup(void)
{
    /*
    // set prescaler to 256
    TCCR1B |= _BV(CS12);
    TCCR1B &= ~(_BV(CS10) | _BV(CS11));
    // set WGM mode 4: CTC using OCR1A
    TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));
    TCCR1B |= _BV(WGM12);
    TCCR1B &= ~_BV(WGM13);
    // normal operation - disconnect PWM pins
    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));
    // set top value for TCNT1
    OCR1A = 10; // just some start value
    // enable COMPA isr
    TIMSK |= _BV(OCIE1A);
    */

    // save some FLASH
    TCCR1A = 0;
    TCCR1B = _BV(CS12) | _BV(WGM12);
    // set top value for TCNT1
    OCR1A = 10; // just some start value
    // enable COMPA isr
    TIMSK |= _BV(OCIE1A);
}

ISR(TIMER1_COMPA_vect) // on attiny2313/4313 this is named TIMER1_COMPA_vect
{
    DISPLAY_OFF;

    static uint16_t bitmask = 0x0001;
    uint8_t OCR1A_next;
    uint8_t led;
    uint8_t tmp_val = 0b00000000;	// off

    for (led = 0; led <= 7; led++) {
        if ( led_brightness[led] & bitmask ) {
            tmp_val |= _BV(led);
        }
    }

    LATCH_LOW;
    spi_transfer(tmp_val);
    LATCH_HIGH;

    OCR1A_next = bitmask;
    bitmask = bitmask << 1;

    if ( bitmask == _BV(BCM_BIT_DEPTH + 1) ) {
        bitmask = 0x0001;
        OCR1A_next = 2;
    }

    OCR1A = OCR1A_next; // when to run next time
    TCNT1 = 0; // clear timer to compensate for code runtime above
    TIFR = _BV(OCF1A); // clear interrupt flag to kill any erroneously pending interrupt in the queue

    DISPLAY_ON;
}

void fade(int16_t fade_from, int16_t fade_to, uint16_t fade_delay)
{
    if( fade_to > LAMP_BRIGHTNESS_MAX) {
        fade_to = LAMP_BRIGHTNESS_MAX;
    }

    if( fade_to < 0 ) {
        fade_to = 0;
    }

    if( fade_from > LAMP_BRIGHTNESS_MAX) {
        fade_from = LAMP_BRIGHTNESS_MAX;
    }

    if( fade_from < 0 ) {
        fade_from = 0;
    }

    if( fade_to > fade_from ) { // fade in
        while( fade_from <= fade_to - MANUAL_FADE_STEPSIZE ) {
                lamp_brightness = fade_from;
                set_led_pattern();
                fade_from += MANUAL_FADE_STEPSIZE;
                delay(fade_delay);
            }
        if( fade_from > fade_to - MANUAL_FADE_STEPSIZE ) { // some odd value too low
            lamp_brightness = fade_to;
            set_led_pattern();
        }
    }
    if( fade_from > fade_to ) { // fade out
        while( fade_from >= fade_to + MANUAL_FADE_STEPSIZE ) {
                lamp_brightness = fade_from;
                set_led_pattern();
                fade_from -= MANUAL_FADE_STEPSIZE;
                delay(fade_delay);
            }
        if( fade_from < fade_to + MANUAL_FADE_STEPSIZE ) { // some odd value left over
            lamp_brightness = fade_to;
            set_led_pattern();
        }
    }
    // nothing to do if both values are equal
}

void up(uint16_t fade_delay)
{
    S_LED_ON;

    if (lamp_brightness <= (LAMP_BRIGHTNESS_MAX - MANUAL_FADE_STEPSIZE)) {
        lamp_brightness = lamp_brightness + MANUAL_FADE_STEPSIZE;
    } else {
        lamp_brightness = LAMP_BRIGHTNESS_MAX;
    }

    set_led_pattern();

    delay(fade_delay); // manually fading in
}

void down(uint16_t fade_delay)
{
    S_LED_OFF;

    if (lamp_brightness >= (0 + MANUAL_FADE_STEPSIZE)) {
        lamp_brightness = lamp_brightness - MANUAL_FADE_STEPSIZE;
    } else {
        lamp_brightness = 0;
    }

    set_led_pattern();

    delay(fade_delay); // manually fading out
}

static void set_led_pattern(void)
{
    if( (lamp_brightness >=0) && (lamp_brightness <=63) ) {
        led_brightness[3] = lamp_brightness;
        led_brightness[2] = 0;
        led_brightness[1] = 0;
        led_brightness[0] = 0;
    }

    if( (lamp_brightness >=64) && (lamp_brightness <=127) ) {
        led_brightness[3] = 63;
        led_brightness[2] = lamp_brightness - 64;
        led_brightness[1] = 0;
        led_brightness[0] = 0;
    }

    if( (lamp_brightness >=128) && (lamp_brightness <=191) ) {
        led_brightness[3] = 63;
        led_brightness[2] = 63;
        led_brightness[1] = lamp_brightness - 128;
        led_brightness[0] = 0;
    }

    if( (lamp_brightness >=192) && (lamp_brightness <=255) ) {
        led_brightness[3] = 63;
        led_brightness[2] = 63;
        led_brightness[1] = 63;
        led_brightness[0] = lamp_brightness - 192;
    }

    led_brightness[4] = led_brightness[3];
    led_brightness[5] = led_brightness[2];
    led_brightness[6] = led_brightness[1];
    led_brightness[7] = led_brightness[0];
}

int16_t get_brightness(void)
{
    return lamp_brightness;
}

uint16_t flash_channel(uint8_t channel, uint8_t times, uint16_t flash_delay)
{
    uint8_t tmp = led_brightness[channel];
    uint8_t ctr;
    for(ctr=0; ctr < times; ctr++) {
        led_brightness[channel] = 0;
        delay(flash_delay);
        led_brightness[channel] = 8;
        delay(flash_delay);
    }
    led_brightness[channel] = tmp;

    return (2*times*flash_delay);
}
