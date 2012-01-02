#include <avr/io.h>
#include <stdint.h>
#include "util.h"
#include "system_ticker.h"

void signal_reset(void)
{
    TOGGLE_LED;
    delay(1000);
    TOGGLE_LED;
    delay(1000);
    TOGGLE_LED;
    delay(1000);
    TOGGLE_LED;
    delay(1000);
}

uint16_t flash_status_led(uint8_t times, uint16_t flash_delay)
{
    uint8_t ctr;
    for(ctr=0; ctr < times; ctr++) {
        LED_ON;
        delay(flash_delay);
        LED_OFF;
        delay(flash_delay);
    }

    return (2*times*flash_delay);
}
