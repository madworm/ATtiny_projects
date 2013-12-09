#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>

#include "system_ticker.hpp"
#include "uart.hpp"
#include "util.hpp"

#define USE_PWM // output 30kHz PWM signal on PB4 (pin-label: 4 / DIR)
//#define SHOW_PWM_MAX // blink onboard LED when PWM has reached 100% duty cycle
#define AUTO_COARSE_FINE // once ticks/s is above a threshold, output 4x the +/- pulses
#define COARSE_FINE_THRESHOLD 35

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
			
			#ifdef AUTO_COARSE_FINE
			if( velocity > COARSE_FINE_THRESHOLD ) {
				soft_uart_send(PSTR("+"),4);
			} else {
				soft_uart_send(PSTR("+"),1);
			}
			#else
			soft_uart_send(PSTR("+"),1);
			#endif

			#ifdef USE_PWM
			if(OCR1B < 255) {
				OCR1B++;
			} else {
				#ifdef SHOW_PWM_MAX
				LED_on; // signal that the maximum has been reached
				delay(200);
				LED_off;
				LED_idle;
				#endif
			}
			lamp_state = 1;
			#endif
		}
		if( counts < 0 ) {

			#ifdef AUTO_COARSE_FINE
			if( velocity < -COARSE_FINE_THRESHOLD ) {
				soft_uart_send(PSTR("-"),4);
			} else {
				soft_uart_send(PSTR("-"),1);
			}
			#else
			soft_uart_send(PSTR("-"),1);
			#endif
			
			#ifdef USE_PWM
			if(OCR1B > 0) {
				OCR1B--;
			} else {
				lamp_state = 0;
			}
			#endif
		}
		if( encoder_get(BUTTON_WAS_PRESSED) ) {
			soft_uart_send(PSTR("/"),1);
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
			soft_uart_send(PSTR("\\"),1);
			not_the_first_time_pressed = 0;
		}
		if( encoder_get(BUTTON_STATE) && ( (button_press_just_happened == 1) || (not_the_first_time_pressed == 1) ) ) {
			// "¯" is an unicode character! It will be sent as 2 bytes ( 0xC2 0xAF )		  
			soft_uart_send(PSTR("¯"),1);
			button_press_just_happened = 0;
			not_the_first_time_pressed = 1;
			delay(50);
		} else {
			//soft_uart_send(PSTR("_"),1);
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
	#ifdef USE_PWM
	setup_pwm();
	#endif
	soft_uart_init();
}
