#include <avr/io.h>
#include <stdint.h>
#include "spi.h"

void usi_setup(void)
{
    DDRB |= _BV(PB4);   // latch pin as output
    DDRB |= _BV(PB6);   // as output (DO)
    DDRB |= _BV(PB7);   // as output (USISCK)
    DDRB &= ~_BV(PB5);  // as input (DI)
    PORTB |= _BV(PB5);  // pullup on (DI)
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
