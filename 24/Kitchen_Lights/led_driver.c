#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "led_driver.h"
#include "util.h"
#include "system_ticker.h"

// for all 8 channels. OCR1A_MAX is fully off, 0 is fully on
uint8_t brightness = OCR1A_MAX; // global variable, see the header file !

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
    OCR1A = OCR1A_MAX;
    // enable COMPA isr
    TIMSK1 |= _BV(OCIE1A);
}

ISR(TIM1_COMPA_vect) // on attiny2313/4313 this is named TIMER1_COMPA_vect
{
    // for debugging only
    PA2_ON;

    // Framebuffer interrupt routine
    DISPLAY_OFF;

    /*
     * init as off and accept that OCR1A_MAX = 0% and 0 = 100%
     * if inited as on, it won't go down to zero brightness
     * which is more annoying than not getting to 100%
     *
     */
    static uint8_t index = 0;
    static uint16_t OCR1A_TMP = 0;

    // starts with index = 0
    // now calculate when to run the next time and turn on all 8-ch

    switch(index) {
    case 0:
        OCR1A_TMP = brightness;
        index++;
        break;
    case 1:
        if ( brightness == OCR1A_MAX) {
            // if the leds should be at 0%, do nothing
        } else {
            DISPLAY_ON; // turn all 8-ch on
        }
        // calculate when to turn everything off
        OCR1A_TMP = ( OCR1A_MAX - brightness );
        index++;
        break;
    case 2:
        // cycle completed, reset everything
        index = 0;
        // immediately restart
        OCR1A_TMP = 0;
        // DON'T increase the index counter !
        break;
    default:
        break;
    }

    OCR1A = OCR1A_TMP;

    // for debugging only
    PA2_OFF;
}

void fade_in(uint8_t start_at, uint16_t fade_delay)
{
    uint8_t ctr1;
    for (ctr1 = start_at; (ctr1 > 0); ctr1--) {
        brightness = ctr1;
        delay(fade_delay);
    }
}

void fade_out(uint8_t start_at, uint16_t fade_delay)
{
    uint8_t ctr1;
    for (ctr1 = start_at; ctr1 <= OCR1A_MAX; ctr1++) {
        brightness = ctr1;
        delay(fade_delay);
    }
}
