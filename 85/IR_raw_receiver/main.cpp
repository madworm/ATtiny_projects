#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

// add this as a GLOBAL define in the build-options to enable it: PIR_MOD
#define PIR_GO_OFF_DELAY 3000000UL

#include "util.hpp"
#include "system_ticker.hpp"
#include "uart.hpp"
#include "IR_receiver.hpp"
#include "main.hpp"

#define USE_BOARD_ADDRESS
#define BOARD_ADDRESS DIGIT_1
#define BOARD_ADDRESS_GENERAL DIGIT_0_OR_10
#define WAIT_FOR_COMMAND_TIMEOUT 4000

int main(void)
{
    setup_hw();

    soft_uart_send(PSTR("Ready!\r\n"));

    while(1) {

#if defined(PIR_MOD) && defined(__AVR_ATtiny85__)
        do_PIR();
#endif

        if ( IR_available() ) {

#if defined(USE_BOARD_ADDRESS)
            IR_code_t IR_code = eval_IR_code();

            if( (IR_code == BOARD_ADDRESS) || (IR_code == BOARD_ADDRESS_GENERAL) ) {

                uint32_t got_valid_board_address_time = millis();

                while( (millis() - got_valid_board_address_time) < WAIT_FOR_COMMAND_TIMEOUT ) {

                    if (IR_available() ) {
#endif
                        switch(eval_IR_code()) {
                        case VOL_UP:
                            soft_uart_send(PSTR("+"));
                            //soft_uart_send(PSTR("volume up\r\n"));
                            break;
                        case VOL_DOWN:
                            soft_uart_send(PSTR("-"));
                            //soft_uart_send(PSTR("volume down\r\n"));
                            break;
                        case ARROW_UP:
                            soft_uart_send(PSTR("F"));
                            //soft_uart_send(PSTR("arrow up\r\n"));
                            break;
                        case ARROW_DOWN:
                            soft_uart_send(PSTR("f"));
                            //soft_uart_send(PSTR("arrow down\r\n"));
                            break;
                        case REPEAT_CODE:
                            //soft_uart_send(PSTR("repeat code\r\n"));
                            break;
                        case MISMATCH:
                            //soft_uart_send(PSTR("UNKNOWN / ERROR ARGH ARGH ARGH!\r\n"));
                            break;
                        case NOT_SURE_YET:
                            //soft_uart_send(PSTR("You should NEVER see this!\r\n"));
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

#if defined(PIR_MOD) && defined(__AVR_ATtiny85__)
    PIR_DIR &= ~_BV(PIR_pin); // set as INPUT
    PIR_PORT &= ~_BV(PIR_pin); // pullup OFF
#else
    LED_DIR |= _BV(LED_pin);
#endif

    system_ticker_setup();
    sei(); // turn global irq flag on, also needed as a wakeup source
    init_IR();
    soft_uart_init();
}

void do_PIR(void)
{
    static uint8_t lamp_state = 0;
    static uint32_t pir_silence = 0;

    uint8_t tmp = PIR_PIN;

    if ( !(tmp & _BV(PIR_pin)) ) {
        pir_silence++;
    } else {
        pir_silence = 0;
    }

    if ( (tmp & _BV(PIR_pin)) && (lamp_state == 0) ) { // lamp is off and PIR sees stuff
        lamp_state = 1;
        pir_silence = 0;
        soft_uart_send(PSTR("F"));
    }

    if ( (lamp_state == 1 ) && (pir_silence > PIR_GO_OFF_DELAY) ) { // lamp is on and PIR has reported silence for more than X minutes
        lamp_state = 0;
        pir_silence = 0;
        soft_uart_send(PSTR("f"));
    }
}
