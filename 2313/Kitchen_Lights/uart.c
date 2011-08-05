#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "uart.h"
#include "util.h"
#include "spi.h"
#include "system_ticker.h"
#include "led_driver.h"

volatile uint8_t uart_rx_byte;     // instantly updated when new byte arrives
volatile uint8_t uart_rx_buffer;   // only overwritten if empty (flag = 0)
volatile uint8_t uart_rx_flag;     // set to 1 when new data in buffer

void uart_setup(void)
{

}

uint8_t uart_read(void)
{
    uint8_t rx_byte = uart_rx_buffer; // get received byte from input buffer
    uart_rx_buffer = 0; // clear input buffer
    uart_rx_flag = 0;  // get ready for next incoming byte

    // only for debugging
    // RX_FLAG_OFF;

    return rx_byte; // send it to the user
}

void uart_send(uint8_t byte)
{

}

uint8_t uart_peek(void)
{
    if(uart_rx_flag) {
        return 1; // new data ready
    } else {
        return 0; // nothing new to read
    }
}

void uart_rx_test(void)
{
    // disable TIMER1_COMPA_vect - prevents flickering
    TIMSK &= ~_BV(OCIE1A);

    uint8_t rx_byte;

    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;

    DISPLAY_OFF;
    LATCH_LOW;
    spi_transfer(0x01); // show LSB
    LATCH_HIGH;
    DISPLAY_ON;

    delay(200);

    while(1) {
        if(uart_rx_flag) {
            rx_byte = uart_read();

            DISPLAY_OFF;
            delay(100); // flicker to signal we got something
            DISPLAY_ON;

            delay(500); // wait before we echo it back
            uart_send('\n');
            uart_send('\r');
            uart_send('e');
            uart_send(':');
            uart_send(' ');
            uart_send(rx_byte); // echo what we 'think' we got
            uart_send('\n');
            uart_send('\r');

            DISPLAY_OFF;
            LATCH_LOW;
            spi_transfer(rx_byte); // show the data
            LATCH_HIGH;
            DISPLAY_ON;
        }
    }
}
