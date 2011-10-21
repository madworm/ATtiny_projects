#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "led_driver.h"
#include "system_ticker.h"

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

}
