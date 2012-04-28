#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include "IR_receiver.hpp"
#include "util.hpp"
#include "uart.hpp"

void soft_uart_init(void)
{
    UART_DIR &= ~_BV(UART_pin); // make it an input
    UART_PORT |= _BV(UART_pin); // pull-up on (idle)

    #if !defined(PIR_MOD)
    LED_ON;
    _delay_ms(500);
    LED_OFF;
    #endif
}

void soft_uart_write(uint8_t byte)
{
    uint8_t ctr;

#if defined(__AVR_ATmega168__)
    UCSR0A = 0; // turn the hardware UART off to release the pins
    UCSR0B = 0;
#endif

    uint8_t _sreg = SREG;
    cli();
    UART_DIR |= _BV(UART_pin);  // make it an output
    UART_PORT &= ~_BV(UART_pin);    // drive it low: start-bit

    _delay_us(HALF_BIT_DELAY);  // _delay_us() can only do 768/F_CPU max !!
    _delay_us(HALF_BIT_DELAY);  // run it twice for safety

    for(ctr=0; ctr<=7; ctr++) {
        if( (byte & _BV(ctr)) ) { // sent byte LSB first
            UART_PORT |= _BV(UART_pin);  // set pin
        } else {
            UART_PORT &= ~_BV(UART_pin); // clear pin
        }
        _delay_us(HALF_BIT_DELAY);
        _delay_us(HALF_BIT_DELAY);
    }
    UART_PORT |= _BV(UART_pin);  // drive it high: stop-bit and later idle
    UART_DIR &= ~_BV(UART_pin); // make it an input (pull-up on)

    SREG = _sreg;

    _delay_ms(1); // don't flood the receiver
}

void soft_uart_send(uint8_t number)
{
    uint8_t a = number/100;
    uint8_t b = (number - a*100)/10;
    uint8_t c = number - a*100 - b*10;

    uint8_t lz = 0;

    if ( a == 0 ) {
        lz = 1;
    } else {
        soft_uart_write(a+'0');
    }

    if ( b == 0 && lz == 1 ) {
    } else {
        soft_uart_write(b+'0');
    }
    soft_uart_write(c+'0');
}

void soft_uart_send(uint8_t number, uint8_t base)
{
    uint8_t bitmask = 0x80;

    if ( base == 10 ) {
        soft_uart_send(number);
    }
    if ( base == 2 ) {
        while (bitmask) {
            if ( number & bitmask ) {
                soft_uart_send((uint8_t)0x01);
            } else {
                soft_uart_send((uint8_t)0x00);
            }
            bitmask >>= 1;
        }
    }
}

void soft_uart_send(uint16_t number)
{
    uint8_t a = number / 10000;
    uint8_t b = (number - 10000*a)/1000;
    uint8_t c = (number - 10000*a - 1000*b)/100;
    uint8_t d = (number - 10000*a - 1000*b -100*c)/10;
    uint8_t e = (number - 10000*a - 1000*b -100*c - 10*d);

    soft_uart_write(a+'0');
    soft_uart_write(b+'0');
    soft_uart_write(c+'0');
    soft_uart_write(d+'0');
    soft_uart_write(e+'0');
}

void soft_uart_send(int16_t number)
{
    if ( number < 0 ) {
        number = (uint16_t)(-number);
    }

    uint8_t a = number / 10000;
    uint8_t b = (number - 10000*a)/1000;
    uint8_t c = (number - 10000*a - 1000*b)/100;
    uint8_t d = (number - 10000*a - 1000*b -100*c)/10;
    uint8_t e = (number - 10000*a - 1000*b -100*c - 10*d);

    soft_uart_write(a+'0');
    soft_uart_write(b+'0');
    soft_uart_write(c+'0');
    soft_uart_write(d+'0');
    soft_uart_write(e+'0');
}

void soft_uart_send(const char * string)
{
    //
    // define strings in flash as such:
    //
    // const PROGMEM char pgm_text[] = "PGM: Hello World!\r\n";
    //

    while(pgm_read_byte(string) != '\0') {
        soft_uart_write(pgm_read_byte(string));
        string++;
    }
}
