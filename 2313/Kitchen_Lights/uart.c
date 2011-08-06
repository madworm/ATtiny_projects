#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "uart.h"
#include "util.h"
#include "spi.h"
#include "system_ticker.h"
#include "led_driver.h"

#define RX_BUFFER_SIZE 8U
#define TX_BUFFER_SIZE 8U

#define BAUDRATE 9600UL
#define UBRR_VAL ( (F_CPU / (16UL * BAUDRATE)) - 1)

#define RX_LED_ON       PORTB |= _BV(PB2)
#define RX_LED_OFF      PORTB &= ~_BV(PB2)
#define RX_LED_TOGGLE   PORTB ^= _BV(PB2)

#define TX_LED_ON       PORTD |= _BV(PD6)
#define TX_LED_OFF      PORTD &= ~_BV(PD6)
#define TX_LED_TOGGLE   PORTD ^= _BV(PD6)

volatile uint8_t rx_buffer[RX_BUFFER_SIZE]; // this is where the RX-ISR writes to
volatile uint8_t rx_buffer_head; // data is ALWAYS written at the head
volatile uint8_t rx_buffer_tail; // data is ALWAYS read at the tail
volatile uint8_t tx_buffer[TX_BUFFER_SIZE]; // this is where the TX-ISR reads from
volatile uint8_t tx_buffer_head; // data is ALWAYS written at the head
volatile uint8_t tx_buffer_tail; // data is ALWAYS read at the tail

void uart_setup(void)
{
    // configure I/O pins
    DDRD &= ~_BV(PD0);  // RXI is an input
    DDRD |= _BV(PD1);   // TXO is an output

    DDRD |= _BV(PD6);   // TX_LED is an output
    DDRB |= _BV(PB2);   // RX_LED is an output

    TX_LED_TOGGLE;  // led test
    TX_LED_TOGGLE;

    RX_LED_TOGGLE;  // led test
    RX_LED_TOGGLE;

    // configure speed. 8N1 is the default
    UBRRL = (uint8_t)(UBRR_VAL);
    UBRRH = (uint8_t)(UBRR_VAL >> 8);
    // enable RX / TX
    UCSRB |= (_BV(RXEN) | _BV(TXEN));
    // enable RX complete ISR
    UCSRB |= _BV(RXCIE);

    // clear all buffers
    uint8_t ctr;
    for(ctr = 0; ctr <= RX_BUFFER_SIZE-1; ctr++)
    {
        rx_buffer[ctr] = 0;
    }
    rx_buffer_head = 0;
    rx_buffer_tail = rx_buffer_head;

    for(ctr = 0; ctr <= TX_BUFFER_SIZE-1; ctr++)
    {
        tx_buffer[ctr] = 0;
    }
    tx_buffer_head = 0;
    tx_buffer_tail = tx_buffer_head;
}

uint8_t uart_avail(void)
{
    // returns the number of available bytes in the rx_buffer
    return (RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE;
}

uint8_t uart_read(void)
{
    // read one byte from the rx_buffer
    uint8_t read_this = rx_buffer_tail;
    if(rx_buffer_tail == rx_buffer_head) {
            // nothing new to read --> return the old data
            // the user should use uart_avail() to check for new data anyway
            // DO NOT increase rx_buffer_tail
    } else {
        rx_buffer_tail = (rx_buffer_tail + 1) % RX_BUFFER_SIZE;
    }
    return rx_buffer[read_this];
}

void uart_send(uint8_t byte)
{
    // tries to put 1 byte into the tx_buffer
    // and enables the sending ISR if successful
    uint8_t tx_buffer_head__incr = (tx_buffer_head + 1) % TX_BUFFER_SIZE;
    while( tx_buffer_head__incr == tx_buffer_tail) { // while increasing the head by 1 WOULD bite into the tail
        // wait until there is some space in the tx_buffer
        // tx_buffer_tail is incremented by 'ISR(USART_UDRE_vect)'
    }
    tx_buffer[tx_buffer_head] = byte;
    tx_buffer_head = tx_buffer_head__incr;
    UCSRB |= _BV(UDRIE); // turn on the ISR to send the stuff out
}

void uart_rx_test(void)
{

}

ISR(USART_RX_vect)
{
    // executed when the hardware gets one byte
    uint8_t rx_buffer_head__incr = (rx_buffer_head + 1) % RX_BUFFER_SIZE;
    if( rx_buffer_head__incr != rx_buffer_tail) { // if increasing the head by 1 WOULD NOT bite into the tail
        // load the incoming byte into the rx_buffer
        rx_buffer[rx_buffer_head] = UDR;
        // advance the head by 1
        rx_buffer_head = rx_buffer_head__incr;
    } else {
        // rx_buffer full --> data was lost
    }
}

ISR(USART_UDRE_vect)
{
    // executed when the hardware tx buffer is empty (ready to send some data)
    if( tx_buffer_tail != tx_buffer_head ) { // if there is some data to send in the software tx_buffer
        UDR = tx_buffer[tx_buffer_tail];
        tx_buffer_tail = (tx_buffer_tail + 1) % TX_BUFFER_SIZE;
    } else {
        UCSRB &= ~_BV(UDRIE); // all done, turn ISR off
    }
}
