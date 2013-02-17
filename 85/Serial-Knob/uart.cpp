#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include "uart.hpp"

void soft_uart_init(void)
{
    UART_DIR &= ~_BV(UART_pin); // make it an input
    UART_PORT |= _BV(UART_pin); // pull-up on (idle)
}

void soft_uart_write(uint8_t byte)
{
    uint8_t ctr;

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

void soft_uart_send(int8_t number)
{
    if ( number < 0 ) {
        number = (uint8_t)(-number);
        soft_uart_write('-');
    }

    uint8_t digit[3];

    digit[0] = number / 100;
    digit[1] = (number - 100*digit[0])/10;
    digit[2] = (number - 100*digit[0] - 10*digit[1]);

    uint8_t counter;
    uint8_t print_now = 0;
    for(counter=0; counter <=2; counter++) {
        if((digit[counter] == 0) && (print_now == 0) && (counter != 2)) {
            continue;
        } else {
            print_now = 1;
            soft_uart_write(digit[counter]+'0');
        }
    }
}

void soft_uart_send(uint16_t number)
{
    uint16_t digit[5];

    digit[0] = number / 10000;
    digit[1] = (number - 10000*digit[0])/1000;
    digit[2] = (number - 10000*digit[0] - 1000*digit[1])/100;
    digit[3] = (number - 10000*digit[0] - 1000*digit[1] -100*digit[2])/10;
    digit[4] = (number - 10000*digit[0] - 1000*digit[1] -100*digit[2] - 10*digit[3]);

    uint8_t counter;
    uint8_t print_now = 0;
    for(counter=0; counter <=4; counter++) {
        if((digit[counter] == 0) && (print_now == 0) && (counter != 4)) {
            continue;
        } else {
            print_now = 1;
            soft_uart_write(digit[counter]+'0');
        }
    }

}

void soft_uart_send(int16_t number)
{
    if ( number < 0 ) {
        number = (uint16_t)(-number);
        soft_uart_write('-');
    }

    uint16_t digit[5];

    digit[0] = number / 10000;
    digit[1] = (number - 10000*digit[0])/1000;
    digit[2] = (number - 10000*digit[0] - 1000*digit[1])/100;
    digit[3] = (number - 10000*digit[0] - 1000*digit[1] -100*digit[2])/10;
    digit[4] = (number - 10000*digit[0] - 1000*digit[1] -100*digit[2] - 10*digit[3]);

    uint8_t counter;
    uint8_t print_now = 0;
    for(counter=0; counter <=4; counter++) {
        if((digit[counter] == 0) && (print_now == 0) && (counter != 4)) {
            continue;
        } else {
            print_now = 1;
            soft_uart_write(digit[counter]+'0');
        }
    }
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
