#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "main.h"

#define BREATHE_MODE

#ifdef BREATHE_MODE
#define BREATHE_DELAY 40
#define BREATHE_DELAY_STEPSIZE 32
#endif // BREATHE_MODE

int main(void)
{
RESTART:

    setup_hw();
    delay(16000);

#ifdef BREATHE_MODE
    while(1) {
        breathe();
    }
#else

    initial_fade_in();

    for (;;) {
        switch (PINB & 0x06) {	// only get PB2 and PB1
        case 0x04:	// PB1 pulled low
            if (OCR0A > 0) {
                OCR0A--; // inverted PWM
            }
            break;
        case 0x02:	// PB2 pulled low
            if (OCR0A < 255) {
                OCR0A++; // inverted PWM
            }
            break;
        case 0x00:	// restart main
            goto RESTART;	// yeah I know... had to be quick and dirty ;-)
            break;
        default:
            break;
        }
        delay(40);
    }

#endif

}

void setup_hw(void)
{
    cli();			// turn interrupts off, just in case

    DDRB |= _BV(PB0);	// set as output for PWM (OC0A)
    DDRB &= ~(_BV(PB1) | _BV(PB2));	// set as input
    PORTB |= _BV(PB1) | _BV(PB2);	// turn on internal pullups
    system_ticker_setup();

    sei();			// turn global irq flag on
}

void initial_fade_in(void)
{
    // inverted PWM
    uint8_t counter;
    for (counter = 255; counter > 192; counter--) {
        OCR0A = counter;
        delay(80);
    }
}

void breathe(void)
{
    static uint16_t breathe_delay = BREATHE_DELAY;

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

    OCR0A = 0;

    while(OCR0A <= 255) {
        if( OCR0A <= 255 - 1 ) {
            OCR0A += 1;
        } else if( OCR0A == 255 ) {
            break;
        } else {
            OCR0A = 255;
        }
        delay(breathe_delay);
    }

    while( OCR0A >= 0 ) {

        if( OCR0A >= 0 + 1 ) {
            OCR0A -= 1;
        } else if( OCR0A == 0 ) {
            break;
        } else {
            OCR0A = 0;
        }
        delay(breathe_delay);
    }
}
