#include <avr/interrupt.h>
#include <stdint.h>
#include "system_ticker.h"

volatile uint16_t system_ticks = 0;

#ifdef BREATHE_MODE
volatile uint16_t breathe_delay = BREATHE_DELAY;
#endif

void system_ticker_setup(void)
{
    // using timer0
    // setting prescaler to 1
    TCCR0B |= _BV(CS00);
    TCCR0B &= ~(_BV(CS01) | _BV(CS02));
    // set to 'FAST PWM' mode
    TCCR0A |= (_BV(WGM01) | _BV(WGM00));
    TCCR0B &= ~_BV(WGM02);
    // inverted PWM on PB0 (OC0A)
    TCCR0A |= _BV(COM0A1);
    TCCR0A |= _BV(COM0A0);
    // enabling overflow interrupt
    TIMSK0 |= _BV(TOIE0);
    OCR0A = 255;		// start fully off (inverted PWM)
}

ISR(TIM0_OVF_vect)		// on attiny2313/4313 this is named TIMER0_OVF_vect
{
    system_ticks++;

#ifdef BREATHE_MODE
    static uint16_t ISR_counter = 0;

    if(ISR_counter == ISR_COUNTER_MAX) {
        switch (PINB & 0x06) {	// only get PB2 and PB1
        case 0x02:	// PB2 pulled low
            if( breathe_delay <= 65535 - BREATHE_DELAY_STEPSIZE ) {
                breathe_delay += BREATHE_DELAY_STEPSIZE;
            } else if( breathe_delay == 65535 ) {
                break;
            } else {
                breathe_delay = 65535;
            }
            break;
        case 0x04:	// PB1 pulled low
            if( breathe_delay >= 0 + BREATHE_DELAY_STEPSIZE ) {
                breathe_delay -= BREATHE_DELAY_STEPSIZE;
            } else if( breathe_delay == 0 ) {
                break;
            } else {
                breathe_delay = 0;
            }
            break;
        default:
            break;
        }
        ISR_counter = 0;
    }
    ISR_counter++;
#endif // BREATHE_MODE
}

uint16_t time(void)
{
    uint8_t _sreg = SREG;
    uint16_t time;
    cli();
    time = system_ticks;
    SREG = _sreg;
    return time;
    
    //return system_ticks;
}

void delay(uint16_t ticks)
{
    uint16_t start_time = time();
    while ((time() - start_time) < ticks) {
        // just wait here
    }
}
