#include <avr/io.h>
#include <stdint.h>
#include "button.h"
#include "util.h"
#include "system_ticker.h"
#include "spi.h"
#include "led_driver.h"

void button_setup()
{
    LEFT_LED_BLINK;   // this also sets the pin direction and pull-up correctly
    MID_LED_BLINK;
    RIGHT_LED_BLINK;
}

void button_test()
{

}

SWITCHES_STATE_t button_read_state()
{
    SWITCHES_STATE_t state = SW_ALL_OPEN;

    if( !(PIND & _BV(PD2)) )
    {
        state = SW_LEFT_PRESSED;
    }
    if( !(PIND & _BV(PD3)) )
    {
        state = SW_MIDDLE_PRESSED;
    }
    if( !(PIND & _BV(PD4)) )
    {
        state = SW_RIGHT_PRESSED;
    }
    if( !(PIND & _BV(PD2)) && !(PIND & _BV(PD3)) )
    {
        state = SW_LEFT_MIDDLE_PRESSED;
    }
    if( !(PIND & _BV(PD4)) && !(PIND & _BV(PD3)) )
    {
        state = SW_RIGHT_MIDDLE_PRESSED;
    }

    return state;
}
