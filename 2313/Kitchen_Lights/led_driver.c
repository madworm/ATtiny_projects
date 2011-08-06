#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "led_driver.h"
#include "util.h"
#include "system_ticker.h"

void led_driver_setup(void)
{
    // set prescaler to 8 for a PWM frequency of about 1kHz (F_CPU = 8000000)
    TCCR1B |= _BV(CS11);
    TCCR1B &= ~(_BV(CS12) | _BV(CS10));
    // set WGM mode 7: FAST PWM 10bit
    TCCR1B &= ~_BV(WGM13);
    TCCR1B |= ~_BV(WGM12);
    TCCR1A |= (_BV(WGM11) | _BV(WGM10));
    // make OC1A (PB3) an output
    DDRB |= _BV(PB3);
    // connect OC1A (PB3) to the PWM generator (wired to the /ENABLE pin of the led driver)
    TCCR1A |= _BV(COM1A1);
    TCCR1A &= ~_BV(COM1A0);
    // set compare match value for TCNT1
    OCR1A = OCR1A_MAX; // 100% duty cycle - all off
}

void fade_in(uint8_t start_at, uint16_t fade_delay)
{
    S_LED_ON;

    uint8_t ctr1;
    for (ctr1 = start_at; (ctr1 > 0); ctr1--) {
        OCR1A = ctr1;
        delay(fade_delay);
    }
}

void fade_out(uint8_t start_at, uint16_t fade_delay)
{
    S_LED_OFF;

    uint8_t ctr1;
    for (ctr1 = start_at; ctr1 <= OCR1A_MAX; ctr1++) {
        OCR1A = ctr1;
        delay(fade_delay);
    }
}

void up(uint16_t fade_delay)
{
    S_LED_ON;

    if (OCR1A > (0 + MANUAL_FADE_STEPSIZE)) {
        OCR1A = OCR1A - MANUAL_FADE_STEPSIZE;
    } else {
        OCR1A = 0;
    }

    delay(fade_delay); // manually fading in
}

void down(uint16_t fade_delay)
{
    S_LED_OFF;

    if (OCR1A < (OCR1A_MAX - MANUAL_FADE_STEPSIZE)) {
        OCR1A = OCR1A + MANUAL_FADE_STEPSIZE;
    } else {
        OCR1A = OCR1A_MAX;
    }

    delay(fade_delay); // manually fading out
}

uint8_t get_brightness(void)
{
    return OCR1A;
}
