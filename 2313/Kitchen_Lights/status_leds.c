#include <avr/io.h>
#include <stdint.h>
#include "status_leds.h"
#include "system_ticker.h"

void status_leds_test(void)
{
    uint8_t ctr;
    for(ctr = 0; ctr < 5; ctr++) {
        RX_LED_BLINK;
        TX_LED_BLINK;
    }

    for(ctr = 0; ctr < 5; ctr++) {
        LEFT_LED_BLINK;
        MID_LED_BLINK;
        RIGHT_LED_BLINK;
    }

    for(ctr = 0; ctr < 5; ctr++) {
        S_LED_BLINK;
    }

}
