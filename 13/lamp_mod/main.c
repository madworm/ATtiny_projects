#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "main.h"

int main(void)
{
    setup_hw();
    for (;;) {
        switch( PINB & 0x06 ) // only get PB2 and PB1
        {
        case 0x02: // something on PB1
            if(OCR0A < 255) {
                OCR0A++;
            }
            break;
        case 0x04: // something on PB2
            if(OCR0A > 0) {
                OCR0A--;
            }
            break;
        default:
            break;
        }
        delay(5);
    }
}

void setup_hw(void)
{
    cli();  // turn interrupts off, just in case

    DDRB |= _BV(PB0); // set as output for PWM (OC0A)
    DDRB &= ~( _BV(PB1) | _BV(PB2) ); // set as input
    system_ticker_setup();

    sei(); // turn global irq flag on
}
