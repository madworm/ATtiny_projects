#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "uart.h"
#include "util.h"
#include "spi.h"
#include "system_ticker.h"
#include "led_driver.h"

volatile uint8_t soft_uart_rx_byte;     // instantly updated when new byte arrives
volatile uint8_t soft_uart_rx_buffer;   // only overwritten if empty (flag = 0)
volatile uint8_t soft_uart_rx_flag;     // set to 1 when new data in buffer

void soft_uart_setup(void)
{
    PCMSK0 |= _BV(PCINT0); // select pin-change interrupt on PA0
    ENABLE_PCINT0_VECT; // make it happen ;-)
}

uint8_t soft_uart_read(void)
{
    uint8_t rx_byte = soft_uart_rx_buffer; // get received byte from input buffer
    soft_uart_rx_buffer = 0; // clear input buffer
    soft_uart_rx_flag = 0;  // get ready for next incoming byte

    // only for debugging
    // RX_FLAG_OFF;

    return rx_byte; // send it to the user
}

void soft_uart_send(uint8_t byte)
{
    DISABLE_PCINT0_VECT; // disable pin-change interrupts on group 0

    uint8_t ctr;

    DDRA |= _BV(PA0); // make it an output
    PORTA &= ~_BV(PA0); // pull it low: start-bit
    _delay_us(FULL_BIT_DELAY);
    for(ctr=0; ctr<=7; ctr++) {
        if( (byte & _BV(ctr)) ) { // sent byte LSB first
            PORTA |= _BV(PA0);  // set pin
        } else {
            PORTA &= ~_BV(PA0); // clear pin
        }
        _delay_us(FULL_BIT_DELAY);
    }
    PORTA |= _BV(PA0);  // pull it high: stop-bit
    DDRA &= ~_BV(PA0);  // make it an input again

    _delay_us(10*FULL_BIT_DELAY); // don't flood the receiver
    _delay_us(10*FULL_BIT_DELAY); // a soft-uart as well

    CLEAR_PCINT0_FLAG;
    ENABLE_PCINT0_VECT; // re-enable pin-change interrupts on group 0
}

uint8_t soft_uart_peek(void)
{
    if(soft_uart_rx_flag) {
        return 1; // new data ready
    } else {
        return 0; // nothing new to read
    }
}

ISR(PCINT0_vect) // pin-change interrupt group 0
{
    // for debugging only
    // PA3_ON;

    // disable pin-change interrupts on group 0 - ASAP !
    // it will be re-enabled once the timer has read the byte
    // or if it didn't get a start-bit
    DISABLE_PCINT0_VECT;

    if( !(PINA & _BV(PA0)) ) {
        // PA0 is low (got a valid start-bit)
        OCR0A = TCNT0 + THREE_HALFS_BIT_DELAY; // TIM0_COMPA_vect should start in the middle in the first data-bit
        CLEAR_TIM0_COMPA_FLAG; // prevent premature execution
        ENABLE_TIM0_COMPA_VECT; // enable the bit sampling
    } else {
        // didn't get a start-bit, re-enable
        CLEAR_PCINT0_FLAG;
        ENABLE_PCINT0_VECT;
    }
    // only for debugging
    // PA3_OFF;
}

ISR(TIM0_COMPA_vect)
{
    // only for debugging
    // PA7_ON;

    static uint8_t bit_value = 1;

    if( (bit_value > 0) && (bit_value <= 0x80) ) { // 0x80 = 0b10000000 - read data-bits 0...7
        if( (PINA & _BV(PA0)) ) {
            soft_uart_rx_byte |= bit_value;
        } else {
            soft_uart_rx_byte &= ~bit_value;
        }
        bit_value <<= 1; // shift 1 bit to the left
        OCR0A = TCNT0 + FULL_BIT_DELAY; // when to run next time
    } else { // stop-bit
        DISABLE_TIM0_COMPA_VECT; // no further runs after this one
        bit_value = 1; // reset bit counter
        if( (PINA & _BV(PA0)) && (soft_uart_rx_flag == 0) ) { // stop-bit is HIGH and buffer empty
            soft_uart_rx_buffer = soft_uart_rx_byte; // copy received byte to input buffer
            soft_uart_rx_flag = 1; // new data! --> set rx-flag
            // only for debugging
            // RX_FLAG_ON;
        }
        CLEAR_PCINT0_FLAG;  // prevent premature execution
        ENABLE_PCINT0_VECT; // turn the receiver back on
    }
    // only for debugging
    // PA7_OFF;
}

void soft_uart_rx_test(void)
{
    // disable TIM1_COMPA_vect - prevents flickering
    TIMSK1 &= ~_BV(OCIE1A);

    uint8_t rx_byte;

    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;

    DISPLAY_OFF;
    spi_transfer(0x01); // show LSB
    LATCH;
    DISPLAY_ON;

    delay(200);

    while(1) {
        if(soft_uart_rx_flag) {
            rx_byte = soft_uart_read();

            DISPLAY_OFF;
            delay(100); // flicker to signal we got something
            DISPLAY_ON;

            delay(500); // wait before we echo it back
            soft_uart_send('\n');
            soft_uart_send('\r');
            soft_uart_send('e');
            soft_uart_send(':');
            soft_uart_send(' ');
            soft_uart_send(rx_byte); // echo what we 'think' we got
            soft_uart_send('\n');
            soft_uart_send('\r');

            DISPLAY_OFF;
            spi_transfer(rx_byte); // show the data
            LATCH;
            DISPLAY_ON;
        }
    }
}
