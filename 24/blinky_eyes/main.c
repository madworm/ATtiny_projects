#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "spi.h"
#include "led_driver.h"
#include "main.h"

int main(void)
{
    setup_hw();
    delay(6000);
    srand(42);
    for (;;) {
        DISPLAY_ON;
        PORTA |= 0x03;
        spi_transfer(0x07);
        LATCH;
        delay(rand()/8);
        PORTA &= ~0x03;
        spi_transfer(0x00);
        LATCH;
        delay(rand()/8);
        DISPLAY_OFF;
    }
}



void setup_hw(void)
{
    cli();  // turn interrupts off, just in case

    // turn all pins to inputs + pull-up on
    // saved about another 0.5mA on my board
    DDRA = 0x00;
    DDRB = 0x00;
    PORTA = 0xFF;
    PORTB = 0xFF;

    /*
     * now configure the pins we actually need
     */

    DDRA |= 0x03;       // turn PA0 and PA1 to outputs
    PORTA &= ~0x03;     // turn PA0 and PA1 off

    DDRB |= _BV(PB0);	// display enable pin as output
    PORTB |= _BV(PB0);	// pullup on
    DISPLAY_OFF; // turn the driver off

    // USI stuff
    DDRB |= _BV(PB1);   // latch pin as output
    PORTB &= ~_BV(PB1);	// latch low

    DDRA |= _BV(PA5);	// as output (DO)
    DDRA |= _BV(PA4);	// as output (USISCK)
    DDRA &= ~_BV(PA6);	// as input (DI)
    PORTA |= _BV(PA6);	// pullup on (DI)

    /*
     * getting ready
     */
    system_ticker_setup();
    led_driver_setup();

    sei(); // turn global irq flag on

    spi_transfer(0x07);	// set wich channels are active
    LATCH;
}
