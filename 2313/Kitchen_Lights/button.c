#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "button.h"
#include "util.h"
#include "system_ticker.h"
#include "spi.h"
#include "led_driver.h"

#define LEFT_LED_ON     DDRD |= _BV(PD2); PORTD &= ~_BV(PD2)
#define LEFT_LED_OFF    DDRD &= ~_BV(PD2); PORTD |= _BV(PD2)
#define LEFT_LED_BLINK  LEFT_LED_OFF; LEFT_LED_ON; delay(1000); LEFT_LED_OFF

#define MID_LED_ON      DDRD |= _BV(PD3); PORTD &= ~_BV(PD3)
#define MID_LED_OFF     DDRD &= ~_BV(PD3); PORTD |= _BV(PD3)
#define MID_LED_BLINK   MID_LED_OFF; MID_LED_ON; delay(1000); MID_LED_OFF

#define RIGHT_LED_ON       DDRD |= _BV(PD4); PORTD &= ~_BV(PD4)
#define RIGHT_LED_OFF      DDRD &= ~_BV(PD4); PORTD |= _BV(PD4)
#define RIGHT_LED_BLINK    RIGHT_LED_OFF; RIGHT_LED_ON; delay(1000); RIGHT_LED_OFF

void button_setup()
{
    sei();              // make the delay() of system_ticker work
    LEFT_LED_BLINK;     // this also sets the pin direction and pull-up correctly
    MID_LED_BLINK;
    RIGHT_LED_BLINK;
    cli();              // turn it off again
}

void button_test()
{
    switch(button_read_state()) {
    case SW_LEFT_PRESSED:
        LATCH_LOW;
        spi_transfer(1);
        spi_transfer(1);
        LATCH_HIGH;
        break;
    case SW_RIGHT_PRESSED:
        LATCH_LOW;
        spi_transfer(128);
        spi_transfer(128);
        LATCH_HIGH;
        break;
    case SW_MIDDLE_PRESSED:
        LATCH_LOW;
        spi_transfer(24);
        spi_transfer(24);
        LATCH_HIGH;
        break;
    case SW_LEFT_MIDDLE_PRESSED:
        LATCH_LOW;
        spi_transfer(24+1);
        spi_transfer(24+1);
        LATCH_HIGH;
        break;
    case SW_RIGHT_MIDDLE_PRESSED:
        LATCH_LOW;
        spi_transfer(128+24);
        spi_transfer(128+24);
        LATCH_HIGH;
        break;
    default:
        LATCH_LOW;
        spi_transfer(0x00);
        spi_transfer(0x00);
        LATCH_HIGH;
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
