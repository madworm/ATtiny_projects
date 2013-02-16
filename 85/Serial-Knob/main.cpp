#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include <util/delay.h>

#include "system_ticker.hpp"
#include "uart.hpp"
#include "util.hpp"
#include "main.hpp"

int main(void)
{
    setup_hw();
   
   	while(1) {
		if( encoder_get() & _BV(BUTTON_WAS_PRESSED) ) {
			LED_on;
			delay(1000);
			LED_off;
			LED_idle;
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

    system_ticker_setup();
    sei(); // turn global irq flag on, also needed as a wakeup source
    soft_uart_init();
}
