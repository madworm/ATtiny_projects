#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "system_ticker.h"
#include "util.h"

volatile uint16_t system_ticks = 0;

void system_ticker_setup(void)
{
    // using timer0
    // setting prescaler to 8
    TCCR0B |= _BV(CS01);
    TCCR0B &= ~(_BV(CS00) | _BV(CS02));
    // set WGM mode 0
    TCCR0A &= ~(_BV(WGM01) | _BV(WGM00));
    TCCR0B &= ~_BV(WGM02);
    // normal operation - disconnect PWM pins
    TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0));
    // enabling overflow interrupt
    TIMSK |= _BV(TOIE0);
}

ISR(TIMER0_OVF_vect) // on attiny2313/4313 this is named TIMER0_OVF_vect
{
    system_ticks++;

    static uint8_t loop = 0;
    if( (PORTD & _BV(PD6)) || (PORTB & _BV(PB2)) ) { // RX or TX LED is on
        loop++;
    }
    if(loop == 32) {
        TX_LED_OFF;
        RX_LED_OFF;
        loop = 0;
    }
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
