#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdint.h>
#include "util.hpp"
#include "uart.hpp"

void soft_uart_init(void)
{
    UART_DIR &= ~_BV(UART_PIN); // make it an input
    UART_PORT |= _BV(UART_PIN); // pull-up on (idle)

    LED_ON;
    _delay_ms(500);
    LED_OFF;
}

void soft_uart_write(uint8_t byte)
{
    uint8_t ctr;

#ifdef __AVR_ATmega168__
    UCSR0A = 0;
    UCSR0B = 0;
#endif

    // the atomic block is needed to make sure this happens
    // _uninterrupted_ by the LED ISR !

    uint8_t _sreg = SREG;
    cli();
    UART_DIR |= _BV(UART_PIN); // make it an output
    UART_PORT &= ~_BV(UART_PIN); // drive it low: start-bit
    _delay_us(FULL_BIT_DELAY);
    for(ctr=0; ctr<=7; ctr++) {
        if( (byte & _BV(ctr)) ) { // sent byte LSB first
            UART_PORT |= _BV(UART_PIN);  // set pin
        } else {
            UART_PORT &= ~_BV(UART_PIN); // clear pin
        }
        _delay_us(FULL_BIT_DELAY);
    }
    UART_PORT |= _BV(UART_PIN);  // drive it high: stop-bit and later idle
    UART_DIR &= ~_BV(UART_PIN); // make it an input (pull-up on)

    SREG = _sreg;

    _delay_us(10*FULL_BIT_DELAY); // don't flood the receiver
    _delay_us(10*FULL_BIT_DELAY); // a soft-uart as well
}

void soft_uart_send(uint8_t number)
{
    uint8_t a = number/100;
    uint8_t b = (number - a*100)/10;
    uint8_t c = number - a*100 - b*10;
    soft_uart_write(a+'0');
    soft_uart_write(b+'0');
    soft_uart_write(c+'0');
}

void soft_uart_send(int8_t number)
{
    if(number < 0) {
        number = (uint8_t)(-number);
        soft_uart_send(PSTR("-"));
    }
    uint8_t a = number/100;
    uint8_t b = (number - a*100)/10;
    uint8_t c = number - a*100 - b*10;
    soft_uart_write(a+'0');
    soft_uart_write(b+'0');
    soft_uart_write(c+'0');
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
    if(number < 0) {
        number = (uint16_t)(-number);
        soft_uart_send(PSTR("-"));
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
