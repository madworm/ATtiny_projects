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

int main(void)
{
    setup_hw();

    soft_uart_send(PSTR("Ready!\r\n"));

    while(1) {

        //TOGGLE_LED;
        //delay(200);

        if ( IR_available() ) {
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
     * Down to 3.6mA for the whole board.
     * The TSOP38238 is said to take 3mA, which leaves 600µA for the AVR.
     * It should go lower, but I can't get it there
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
