#include <avr/interrupt.h>
#include <stdint.h>
#include "system_ticker.h"

volatile uint16_t system_ticks = 0; // this is a global variable, see the header file !

void setup_system_ticker(void)
{
    // using timer0
    // setting prescaler to 8
    // 1MHz clock is required for TIM0_COMPA_vect !!!!
    TCCR0B |= _BV(CS01);
    TCCR0B &= ~(_BV(CS00) | _BV(CS02));
    // set WGM mode 0
    TCCR0A &= ~(_BV(WGM01) | _BV(WGM00));
    TCCR0B &= ~_BV(WGM02);
    // normal operation - disconnect PWM pins
    TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0));
    // enabling overflow interrupt
    TIMSK0 |= _BV(TOIE0);
}

ISR(TIM0_OVF_vect) // on attiny2313/4313 this is named TIMER0_OVF_vect
{
    system_ticks++;
}

uint16_t time(void)
{
    uint8_t _sreg = SREG;
    uint16_t time;
    cli();
    time = system_ticks;
    SREG = _sreg;
    return time;
}

void delay(uint16_t ticks)
{
    uint16_t start_time = time();
    while ((time() - start_time) < ticks) {
        // just wait here
    }
}
