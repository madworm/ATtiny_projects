#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "button.h"
#include "system_ticker.h"
#include "spi.h"
#include "led_driver.h"
#include "status_leds.h"

void button_setup()
{
    LEFT_LED_OFF;       // this also sets the pin direction and pull-up correctly
    MID_LED_OFF;        // so the buttons can be read
    RIGHT_LED_OFF;
}

void button_test()
{
    switch(button_read_state()) {
    case SW_LEFT_PRESSED:
        spi_transfer(1);
        LATCH;
        break;
    case SW_RIGHT_PRESSED:
        spi_transfer(128);
	LATCH;
        break;
    case SW_MIDDLE_PRESSED:
        spi_transfer(24);
	LATCH;
        break;
    case SW_LEFT_MIDDLE_PRESSED:
        spi_transfer(24+1);
	LATCH;
        break;
    case SW_RIGHT_MIDDLE_PRESSED:
        spi_transfer(128+24);
      	LATCH;
        break;
    default:
        spi_transfer(0x00);
        LATCH;
        break;
    }
}

SWITCHES_STATE_t button_read_state()
{
    SWITCHES_STATE_t state = SW_ALL_OPEN;

    if( !(PIND & _BV(PD2)) && (PIND & _BV(PD3)) && (PIND & _BV(PD4)) )
    {
        state = SW_LEFT_PRESSED;
    }
    if( (PIND & _BV(PD2)) && !(PIND & _BV(PD3)) && (PIND & _BV(PD4)) )
    {
        state = SW_MIDDLE_PRESSED;
    }
    if( (PIND & _BV(PD2)) && (PIND & _BV(PD3)) && !(PIND & _BV(PD4)) )
    {
        state = SW_RIGHT_PRESSED;
    }
    if( !(PIND & _BV(PD2)) && !(PIND & _BV(PD3)) && (PIND & _BV(PD4)) )
    {
        state = SW_LEFT_MIDDLE_PRESSED;
    }
    if( (PIND & _BV(PD2)) && !(PIND & _BV(PD3)) && !(PIND & _BV(PD4)) )
    {
        state = SW_RIGHT_MIDDLE_PRESSED;
    }

    return state;
}
