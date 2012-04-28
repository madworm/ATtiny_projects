#include <avr/io.h>
#include <stdint.h>
#include "spi.h"

void usi_setup(void)
{
    /*
    DDRB |= _BV(PB4);   // latch pin as output
    PORTB &= ~_BV(PB4);  // latch low
    DDRB |= _BV(PB6);   // as output (DO)
    DDRB |= _BV(PB7);   // as output (USISCK)
    DDRB &= ~_BV(PB5);  // as input (DI)
    PORTB |= _BV(PB5);  // pullup on (DI)
    */

    /*
     * save some FLASH - relying on default register values
     *
     * DDRB should be all 0 after reset
     * PORTB should be all 0 after reset
     *
     * --> all GPIO pins: input, NO pullup
     *
     */

    DDRB |= _BV(PB4) | _BV(PB6) | _BV(PB7);
    PORTB |= _BV(PB5);
}

uint8_t spi_transfer(uint8_t data)
{
    USIDR = data;
    USISR = _BV(USIOIF);    // clear flag

    while ((USISR & _BV(USIOIF)) == 0) {
        USICR = (1 << USIWM0) | (1 << USICS1) | (1 << USICLK) | (1 << USITC);
    }
    return USIDR;
}
