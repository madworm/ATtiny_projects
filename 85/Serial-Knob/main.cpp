#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>

#include "system_ticker.hpp"
#include "uart.hpp"
#include "util.hpp"

//#define USE_PWM // output 30kHz PWM signal on PB4 (pin-label: 4 / DIR)
//#define VELOCITY_BLAME // send a note if turning faster than some value
#define VELOCITY_BLAME_VALUE 50

#ifdef USE_PWM
#include "pwm.hpp"
#endif

#include "main.hpp"

int main(void)
{
    setup_hw();

	static uint8_t button_press_just_happened = 0;
	static uint8_t not_the_first_time_pressed = 0;
	#ifdef USE_PWM
	uint8_t lamp_state = 0;
	uint8_t OCR1B_tmp = 0;
	#endif

	while(1) {
		int8_t counts = encoder_get(ENC_COUNTS);
		int16_t velocity = encoder_get(ENC_VELOCITY);

		if( counts > 0 ) {
			soft_uart_send(PSTR("+"));
			#ifdef USE_PWM
			if(OCR1B < 255) {
				OCR1B++;
			} else {
				LED_on; // signal that the maximum has been reached
				delay(500);
				LED_off;
				LED_idle;
			}
			lamp_state = 1;
			#endif
		}
		if( counts < 0 ) {
			soft_uart_send(PSTR("-"));
			#ifdef USE_PWM
			if(OCR1B > 0) {
				OCR1B--;
			} else {
				lamp_state = 0;
			}
			#endif
		}
		if( encoder_get(BUTTON_WAS_PRESSED) ) {
			soft_uart_send(PSTR("/"));
			button_press_just_happened = 1;
			#ifdef USE_PWM
			if( lamp_state == 1 ) {
				OCR1B_tmp = OCR1B; // save PWM value
				lamp_state = 0; // lamp is off
				OCR1B = 0; // turn PWM off
			} else {
				OCR1B = OCR1B_tmp; // restore PWM value
				lamp_state = 1; // lamp is on
			}
			#endif
		}
		if( encoder_get(BUTTON_WAS_RELEASED) ) {
			soft_uart_send(PSTR("\\"));
			not_the_first_time_pressed = 0;
		}
		if( encoder_get(BUTTON_STATE) && ( (button_press_just_happened == 1) || (not_the_first_time_pressed == 1) ) ) {
			soft_uart_send(PSTR("¯"));
			button_press_just_happened = 0;
			not_the_first_time_pressed = 1;
			delay(50);
		} else {
			//soft_uart_send(PSTR("_"));
		}
		#ifdef VELOCITY_BLAME	
		if( (velocity > VELOCITY_BLAME_VALUE) || (velocity < -VELOCITY_BLAME_VALUE) ) {
			soft_uart_send(PSTR(" ! 50 ticks/s ! "));
			LED_on;
			delay(2000);
			LED_off;
			LED_idle;
		}
		#endif
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
	#ifdef USE_PWM
	setup_pwm();
	#endif
	soft_uart_init();
}
