#include <avr/io.h>
#include <stdint.h>
#include "spi.h"

uint8_t spi_transfer(uint8_t data)
{
    USIDR = data;
    USISR = _BV(USIOIF);    // clear flag

    while ((USISR & _BV(USIOIF)) == 0) {
        USICR = (1 << USIWM0) | (1 << USICS1) | (1 << USICLK) | (1 << USITC);
    }
    return USIDR;
}
