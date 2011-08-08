#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
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
#define RX_LED_BLINK    RX_LED_ON; RX_LED_OFF
#define RX_LED_TOGGLE   PORTB ^= _BV(PB2)

#define TX_LED_ON       PORTD |= _BV(PD6)
#define TX_LED_OFF      PORTD &= ~_BV(PD6)
#define TX_LED_BLINK    TX_LED_ON; TX_LED_OFF
#define TX_LED_TOGGLE   PORTD ^= _BV(PD6)

#define HALF_DUPLEX // kills local echo when RXI and TXO are wired together

volatile uint8_t rx_buffer[RX_BUFFER_SIZE]; // this is where the RX-ISR writes to
volatile uint8_t rx_buffer_head; // data is ALWAYS written at the head
volatile uint8_t rx_buffer_tail; // data is ALWAYS read at the tail
volatile uint8_t tx_buffer[TX_BUFFER_SIZE]; // this is where the TX-ISR reads from
volatile uint8_t tx_buffer_head; // data is ALWAYS written at the head
volatile uint8_t tx_buffer_tail; // data is ALWAYS read at the tail

void uart_setup(void)
{
    // configure I/O pins
    // directions of the RXI / TXO pins are overridden by the RXEN / TXEN bits !

    DDRD |= _BV(PD6);   // TX_LED is an output
    DDRB |= _BV(PB2);   // RX_LED is an output

    TX_LED_TOGGLE;  // led test
    TX_LED_TOGGLE;

    RX_LED_TOGGLE;  // led test
    RX_LED_TOGGLE;

    // configure speed. 8N1 is the default
    UBRRL = (uint8_t)(UBRR_VAL);
    UBRRH = (uint8_t)(UBRR_VAL >> 8);
    // enable RX
    UCSRB |= _BV(RXEN);
    // enable RX complete ISR
    UCSRB |= _BV(RXCIE);

    // clear all buffers
    uint8_t ctr;
    for(ctr = 0; ctr <= RX_BUFFER_SIZE-1; ctr++) {
        rx_buffer[ctr] = 0;
    }
    rx_buffer_head = 0;
    rx_buffer_tail = rx_buffer_head;

    for(ctr = 0; ctr <= TX_BUFFER_SIZE-1; ctr++) {
        tx_buffer[ctr] = 0;
    }
    tx_buffer_head = 0;
    tx_buffer_tail = tx_buffer_head;
}

uint8_t uart_rx_buffer_fullness(void)
{
    // returns the number of available bytes in the rx_buffer

    return (RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE;
}

uint8_t uart_tx_buffer_fullness(void)
{
    // returns the number of available bytes in the rx_buffer

    return (TX_BUFFER_SIZE + tx_buffer_head - tx_buffer_tail) % TX_BUFFER_SIZE;
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
    UCSRB |= _BV(TXEN); // only turn the hardware on when really needed, grab the TXO line
    tx_buffer[tx_buffer_head] = byte;
    tx_buffer_head = tx_buffer_head__incr;
    UCSRB |= _BV(UDRIE); // turn on the ISR to send the stuff out
}

void uart_send_string(const char * string)
{
    //
    // define strings in RAM as such:
    //
    // const char string[] = "Hello World!\r\n"
    //

    while(*string != '\0') {
        uart_send(*string);
        string++;
    }
}

void uart_send_pgm_string(const char * string)
{
    //
    // define strings in flash as such:
    //
    // const PROGMEM char pgm_text[] = "PGM: Hello World!\r\n";
    //

    while(pgm_read_byte(string) != '\0') {
        uart_send(pgm_read_byte(string));
        string++;
    }
}

ISR(USART_RX_vect)
{
    // executed when the hardware gets one byte

    uint8_t clear;
    uint8_t rx_buffer_head__incr = (rx_buffer_head + 1) % RX_BUFFER_SIZE;
    #ifdef HALF_DUPLEX
    // only copy received data into the rx_buffer when:
    // * there is enough space in the rx_buffer
    // * the tx_buffer is empty (we're not in the middle of sending out stuff)
    // * we did NOT just send out a byte (TX-complete flag not set)
    if( (rx_buffer_head__incr != rx_buffer_tail) && (tx_buffer_head == tx_buffer_tail) && !(UCSRA & _BV(TXC)) ) {
    #else
    if( (rx_buffer_head__incr != rx_buffer_tail) ) { // if increasing the head by 1 WOULD NOT bite into the tail
    #endif
        RX_LED_BLINK;
        // load the incoming byte into the rx_buffer
        rx_buffer[rx_buffer_head] = UDR;
        // advance the head by 1
        rx_buffer_head = rx_buffer_head__incr;
    } else {
        // rx_buffer full --> data will not be read into the rx_buffer!
        // perform a dummy read to clear UDR !!
        // if this is not done, the ISR(USART_UDRE_vect) will not fire for sending --> BANG!
        clear = UDR;
    }
    #ifdef HALF_DUPLEX
    UCSRA |= _BV(TXC); // clear TX-complete flag by writing a 1 to it
    #endif
}

ISR(USART_UDRE_vect)
{
    // executed when the hardware tx buffer is empty (ready to send some data)

    if( tx_buffer_tail != tx_buffer_head ) { // if there is some data to send in the software tx_buffer
        TX_LED_BLINK;
        UDR = tx_buffer[tx_buffer_tail];
        tx_buffer_tail = (tx_buffer_tail + 1) % TX_BUFFER_SIZE;
        // when the transmission of a single byte is done, the TXC flag will be set in UCSRA
    } else {
        UCSRB &= ~_BV(UDRIE);   // all done, turn ISR off
        UCSRB &= ~_BV(TXEN);    // turn off the hardware, release the TXO line
    }
}

void uart_half_duplex_test(void)
{
    // test with RXI wired to TXO

    OCR1A = OCR1A_MAX - 10;

    uart_send_pgm_string(PSTR("Hello World from an ATtiny2313!\r\n"));
    //delay(100); // depending on the delay, the tx_buffer will show up more or less full/empty
    spi_transfer(uart_rx_buffer_fullness());
    spi_transfer(uart_tx_buffer_fullness());
    LATCH;
    if( uart_avail() ) {
        // only in full duplex mode whe should have got here
        S_LED_ON;
        delay(200);
        uart_read();
        S_LED_OFF;
        delay(1000);
    } else {
        // in HALF_DUPLEX mode, the led should always blink 2
        S_LED_ON;
        delay(200);
        S_LED_OFF;
        delay(200);
        S_LED_ON;
        delay(200);
        S_LED_OFF;
        delay(1000);
    }
}
