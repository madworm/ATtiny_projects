#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>
#include "config.h"
#include "util.h"
#include "uart.h"

volatile uint8_t soft_uart_rx_byte;     // instantly updated when new byte arrives
volatile uint8_t soft_uart_rx_buffer;   // only overwritten if empty (flag = 0)
volatile uint8_t soft_uart_rx_flag;     // set to 1 when new data in buffer

void soft_uart_setup(void)
{
    DDRB &= ~_BV(PB2); // PB2 as input
    PORTB |= _BV(PB2); // pull-up on
    PCMSK |= _BV(PCINT2); // select pin-change interrupt on PB2
    ENABLE_PCINT0_VECT; // make it happen ;-)
}

uint8_t soft_uart_read(void)
{
    uint8_t rx_byte = soft_uart_rx_buffer; // get received byte from input buffer
    soft_uart_rx_buffer = 0; // clear input buffer
    soft_uart_rx_flag = 0;  // get ready for next incoming byte

    return rx_byte; // send it to the user
}

void soft_uart_send(uint8_t byte, uint8_t half_bit_delay)
{
    DISABLE_PCINT0_VECT; // disable pin-change interrupts on group 0

    uint8_t ctr;

    // the atomic block is needed to make sure this happens
    // _uninterrupted_ by the LED ISR !

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        DDRB |= _BV(PB2); // make it an output
        PORTB &= ~_BV(PB2); // pull it low: start-bit

        delay_us(half_bit_delay);
        delay_us(half_bit_delay);

        for(ctr=0; ctr<=7; ctr++) {
            if( (byte & _BV(ctr)) ) { // sent byte LSB first
                PORTB |= _BV(PB2);  // set pin
            } else {
                PORTB &= ~_BV(PB2); // clear pin
            }
            delay_us(half_bit_delay);
            delay_us(half_bit_delay);
        }
        PORTB |= _BV(PB2);  // pull it high: stop-bit
        DDRB &= ~_BV(PB2);  // make it an input again
    }

    delay_ms(1); // don't flood the receiver - a soft-uart as well

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
    // disable pin-change interrupts on group 0 - ASAP !
    // it will be re-enabled once the timer has read the byte
    // or if it didn't get a start-bit
    DISABLE_PCINT0_VECT;

	// mark when ISR fires: WORKS as expected
	//pulse_PB1();

 	/* 
	// visually check if the irq fires: YES	
	uint8_t ctr = 5;
    while(ctr) {
		pulse_PB0_ms(50);
		ctr--;
	}
	*/

    if( !(PINB & _BV(PB2)) ) {
        // PB2 is low (got a valid start-bit)
        CLEAR_TIM0_COMPB_FLAG; // prevent premature execution
        ENABLE_TIM0_COMPB_VECT; // enable the bit sampling
        OCR0B = TCNT0 + 3*HALF_BIT_DELAY; // TIM0_COMPB_vect should start in the middle in the first data-bit
	} else {
        // didn't get a start-bit, re-enable
        CLEAR_PCINT0_FLAG;
        ENABLE_PCINT0_VECT;
    }
}

ISR(TIM0_COMPB_vect)
{
	/*
	// visually check if the irq fires: YES, but correct timing ??	
	uint8_t ctr = 5;
    while(ctr) {
		pulse_PB0_ms(50);
		ctr--;
	}
	*/

	// add pulse for logic-analyzer testing of bit-sample timing
	//pulse_PB1();
	
	//
	// ISR only fires correctly with TCNT0 reset... WHY?
	// 		TCNT0 = 0;
	// 		OCR0B = 2 x HALF_BIT_DELAY;
	//
	// THIS doesn't work, but should:
	//		OCR0B = TCNT0 + 2 x HALF_BIT_DELAY;	
	//
	// OK... RTFM... timer0 was set to 'FAST PWM'
	// FAST PWM (WGM 0): Update of OCRx at 'TOP' --> 1 full run worst case before changes take place
	// NORMAL (WGM 4)  : Update of OCRx at 'IMMEDIATE' --> that's what we want!
	//

    static uint8_t bit_value = 1;

    if( (bit_value > 0) && (bit_value <= 0x80) ) { // 0x80 = 0b10000000 - read data-bits 0...7
        if( (PINB & _BV(PB2)) ) {
            soft_uart_rx_byte |= bit_value;
        } else {
            soft_uart_rx_byte &= ~bit_value;
        }
        bit_value <<= 1; // shift 1 bit to the left
        OCR0B = TCNT0 + 2*HALF_BIT_DELAY; // when to run next time
    } else { // stop-bit
        DISABLE_TIM0_COMPB_VECT; // no further runs after this one
        bit_value = 1; // reset bit counter
        if( (PINB & _BV(PB2)) && (soft_uart_rx_flag == 0) ) { // stop-bit is HIGH and buffer empty
            soft_uart_rx_buffer = soft_uart_rx_byte; // copy received byte to input buffer
            soft_uart_rx_flag = 1; // new data! --> set rx-flag
        }
        CLEAR_PCINT0_FLAG;  // prevent premature execution
        ENABLE_PCINT0_VECT; // turn the receiver back on
    }
}
