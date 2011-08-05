#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "button.h"
#include "util.h"
#include "system_ticker.h"
#include "spi.h"
#include "led_driver.h"

void button_test()
{
    // disable COMPA isr - prevents flickering
    TIMSK &= ~_BV(OCIE1A);

    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;

    DISPLAY_OFF;
    LATCH_LOW;
    spi_transfer(0x01); // show LSB
    LATCH_HIGH;
    DISPLAY_ON;

    delay(200);

    while(1) {
        uint8_t tmp = button_read();

        DISPLAY_OFF;
        LATCH_LOW;
        spi_transfer(tmp); // show the data
        LATCH_HIGH;
        DISPLAY_ON;

        DISPLAY_OFF;
        LATCH_LOW;
        spi_transfer(0x00); // turn it off to make it less blindingly bright
        LATCH_HIGH;
        DISPLAY_ON;
    }
}

uint8_t button_read()
{
    return 0;
}

SWITCHES_STATE_t button_read_state()
{
    uint8_t state = SW_ALL_OPEN;
    return state;
}
