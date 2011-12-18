#include <avr/io.h>
#include <stdint.h>
#include "status_leds.h"
#include "system_ticker.h"

void status_leds_test(void)
{
        RX_LED_ON;
        TX_LED_ON;
        LEFT_LED_ON;
        MID_LED_ON;
        RIGHT_LED_ON;
        S_LED_ON;

        delay(4000);

        RX_LED_OFF;
        TX_LED_OFF;
        LEFT_LED_OFF;
        MID_LED_OFF;
        RIGHT_LED_OFF;
        S_LED_OFF;
}
