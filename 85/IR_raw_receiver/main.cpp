#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <inttypes.h>

#include "util.hpp"
#include "system_ticker.hpp"
#include "uart.hpp"
#include "IR_receiver.hpp"
#include "main.hpp"

#define USE_BOARD_ADDRESS
#define BOARD_ADDRESS DIGIT_1
#define BOARD_ADDRESS_GENERAL DIGIT_0_OR_10
#define WAIT_FOR_COMMAND_TIMEOUT 3000

int main(void)
{
    setup_hw();

    soft_uart_send(PSTR("Ready!\r\n"));

    while(1) {

        //TOGGLE_LED;
        //delay(200);

        if ( IR_available() ) {

#ifdef USE_BOARD_ADDRESS
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
#ifdef USE_BOARD_ADDRESS
                    }
                }
            }
#endif
        } else {
            /*
             * This is only enabled, if IR_available() has set called 'sleep_enable()'
             * Wakeup and 'sleep_disable()' is done in the pin-change-ISR
             */
            sleep_bod_disable();
            sleep_cpu();
        }
    }
    return 0;
}

void setup_hw(void)
{
    /*
     * save some power
     *
     * Down to 3.6mA for the whole board (idle).
     *
     * The AVR takes about 20µA in shutdown, the TSOP38238 a couple of 100µA
     * when no IR is detected, the rest is the L78L05. The datasheet claims
     * about 6mA of quiescent current, I see about 3mA (12V input).
     */

    DDRB = 0x00; // all inputs
    PORTB = 0xFF; // all pull-ups on
    ACSR |= _BV(ACD); // disable the analog comparator, ACIE is already zero (default value)
    PRR |= _BV(PRTIM1) | _BV(PRUSI) | _BV(PRADC); // turn off the clock for TIMER1, USI and ADC
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable(); // Enable for the first time only. Subsequent times see the pin-change ISR / IR_available()

    // configure what we need to use

    LED_DIR |= _BV(LED_PIN);
    system_ticker_setup();
    sei(); // turn global irq flag on, also needed as a wakeup source
    init_IR();
    soft_uart_init();
}

void read_mcusr(void)
{
    my_mcusr = MCUSR;
    MCUSR = 0;
}
