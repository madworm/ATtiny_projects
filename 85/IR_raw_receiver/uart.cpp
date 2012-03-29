#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdint.h>

#define BAUDSPEED 9600 // valid: 9600, 38400
#include "uart.hpp"

#ifdef __AVR_ATmega168__
#define UART_PORT PORTD
#define UART_DIR  DDRD
#define UART_PIN PD1 // Arduino digital pin #1 (tx)
#endif
#ifdef __AVR_ATtiny85__
#define UART_PORT PORTB
#define UART_DIR  DDRB
#define UART_PIN PB0
#endif

void soft_uart_init(void) {
    UART_DIR |= _BV(UART_PIN); // make it an output
    UART_PORT |= _BV(UART_PIN);  // pull it high
    _delay_ms(500);
    UART_DIR &= ~_BV(UART_PIN);  // make it an input again
    UART_PORT &= ~_BV(UART_PIN); // pull-up off
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
    UART_PORT &= ~_BV(UART_PIN); // pull it low: start-bit
    _delay_us(FULL_BIT_DELAY);
    for(ctr=0; ctr<=7; ctr++) {
        if( (byte & _BV(ctr)) ) { // sent byte LSB first
            UART_PORT |= _BV(UART_PIN);  // set pin
        } else {
            UART_PORT &= ~_BV(UART_PIN); // clear pin
        }
        _delay_us(FULL_BIT_DELAY);
    }
    UART_PORT |= _BV(UART_PIN);  // pull it high: stop-bit
    UART_DIR &= ~_BV(UART_PIN);  // make it an input again
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
