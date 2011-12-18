#include <avr/io.h>
#include <stdint.h>
#include "status_leds.h"
#include "system_ticker.h"

uint8_t led_test;

void status_leds_test(void)
{
        led_test = 1; // tell the system_ticker ISR __not__ to switch RX/TX off

        RX_LED_ON;
        TX_LED_ON;
        LEFT_LED_ON;
        MID_LED_ON;
        RIGHT_LED_ON;
        S_LED_ON;

        delay(6000);

        RX_LED_OFF;
        TX_LED_OFF;
        LEFT_LED_OFF;
        MID_LED_OFF;
        RIGHT_LED_OFF;
        S_LED_OFF;

        led_test = 0;
}

uint16_t flash_status_led(uint8_t times, uint16_t flash_delay)
{
    uint8_t ctr;
    for(ctr=0; ctr < times; ctr++) {
        S_LED_ON;
        delay(flash_delay);
        S_LED_OFF;
        delay(flash_delay);
    }

    return (2*times*flash_delay);
}
