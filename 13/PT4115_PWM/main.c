#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "main.h"

int main(void)
{
#ifndef BREATHE_MODE
RESTART:
#endif

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
	    /*
	    if (OCR0A == 253) {
	        // hopefull eliminate minimum brightness
	        // inconsistency for one board
	    	OCR0A = 255;
	    }
	    */
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
        delay(750); // use about 2000 if you need precise control of brightness
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

#ifdef BREATHE_MODE
void breathe(void)
{
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
#endif // BREATHE_MODE
