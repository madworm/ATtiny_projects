#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include <util/delay.h>

#include "system_ticker.hpp"
#include "uart.hpp"
#include "IR_receiver.hpp"
#include "main.hpp"

//#define IR_SCAN_ON_POWERUP_ENABLED

//#define USE_BOARD_ADDRESS
#define BOARD_ADDRESS DIGIT_1
#define BOARD_ADDRESS_GENERAL DIGIT_0_OR_10
#define WAIT_FOR_COMMAND_TIMEOUT 4000

int main(void)
{
    setup_hw();

#ifdef IR_SCAN_ON_POWERUP_ENABLED
	// if there is any IR data within 5 seconds of power-on
	// we go into scan mode and transmit pulse-data on the
	// SCK-pin of the ISP header (9600,8,N,1).
	while(millis() < 5000) {
		if( IR_available() ) {
			while(1) {
				IR_scan();
			}
		}
	}
#endif

	while(1) {

		if ( IR_available() ) {

#if defined(USE_BOARD_ADDRESS)
            IR_code_t IR_code = eval_IR_code();

            if( (IR_code == BOARD_ADDRESS) || (IR_code == BOARD_ADDRESS_GENERAL) ) {

                uint32_t got_valid_board_address_time = millis();

                while( (millis() - got_valid_board_address_time) < WAIT_FOR_COMMAND_TIMEOUT ) {

                    if (IR_available() ) {
#endif
                        switch(eval_IR_code()) {
                        case VOL_UP: // inverted PWM
                            if(OCR0A >0) {
                                OCR0A--;
                            } else {
                                OCR0A = 0;
                            }
                            break;
                        case VOL_DOWN: // inverted PWM
                            if(OCR0A < 255) {
                                OCR0A++;
                            } else {
                                OCR0A = 255;
                            }
                            break;
                        case ARROW_UP:
                            OCR0A = 0; // inverted PWM
                            break;
                        case ARROW_DOWN:
                            OCR0A = 255; // inverted PWM
                            break;
                        case REPEAT_CODE:
                            break;
                        case MISMATCH:
                            break;
                        case NOT_SURE_YET:
                            break;
                        default:
                            break;
                        }
#if defined(USE_BOARD_ADDRESS)
                    }
                }
            }
#endif
        }
    }
    return 0;
}

void setup_hw(void)
{
    /*
     * save some power
     *
     */

    DDRB = 0x00; // all inputs
    PORTB = 0xFF; // all pull-ups on
    ACSR |= _BV(ACD); // disable the analog comparator, ACIE is already zero (default value)
    PRR |= _BV(PRTIM1) | _BV(PRUSI) | _BV(PRADC); // turn off the clock for TIMER1, USI and ADC

    // configure what we need to use
    PORTB &= ~_BV(PB0); // pull-up on PWM pin off
    DDRB |= _BV(PB0); // PWM output pin

    system_ticker_setup();
    sei(); // turn global irq flag on, also needed as a wakeup source
    init_IR();
    soft_uart_init();
}
