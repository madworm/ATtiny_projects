#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "system_ticker.h"
#include "uart.h"
#include "IR_receiver.h"
#include "main.h"

int main(void)
{
    setup_hw();

    soft_uart_send_PSTR(PSTR("Ready!\r\n"));

    while(1) {
        if ( IR_available() ) {
            switch(eval_IR_code()) {
            case VOL_UP:
                soft_uart_send_PSTR(PSTR("volume up\r\n"));
                break;
            case VOL_DOWN:
                soft_uart_send_PSTR(PSTR("volume down\r\n"));
                break;
            case ARROW_UP:
                soft_uart_send_PSTR(PSTR("arrow up\r\n"));
                break;
            case ARROW_DOWN:
                soft_uart_send_PSTR(PSTR("arrow down\r\n"));
                break;
            case REPEAT_CODE:
                soft_uart_send_PSTR(PSTR("repeat code\r\n"));
                break;
            case MISMATCH:
                soft_uart_send_PSTR(PSTR("UNKNOWN / ERROR ARGH ARGH ARGH!\r\n"));
                break;
            case NOT_SURE_YET:
                soft_uart_send_PSTR(PSTR("You should NEVER see this!\r\n"));
                break;
            default:
                break;
            }
        }
    }
}

void setup_hw(void)
{
    system_ticker_setup();
    sei(); // turn global irq flag on
    init_IR();
}

void read_mcusr(void)
{
    my_mcusr = MCUSR;
    MCUSR = 0;
}
