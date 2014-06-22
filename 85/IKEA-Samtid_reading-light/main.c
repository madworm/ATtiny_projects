#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "system_ticker.h"
#include "adc.h"
#include "pwm.h"
#include "main.h"

int main(void)
{
	setup_hw();

	initial_fade_in();
	
	while (1) {
		uint16_t adc_val = adc_read();
		if( adc_val > 562 ) {
			if(OCR1B < 255) {
				OCR1B++;
			}
		} else if ( adc_val < 462 ) {
			if(OCR1B > 0) {
			OCR1B--;
			}
		}
		delay(25);
	}

}

void setup_hw(void)
{
	// todo
	//
	// pin directions...
	//
	system_ticker_init();
	adc_init();
	pwm_init();
	sei();
}

void initial_fade_in(void)
{
    uint8_t counter;
    for (counter = 0; counter < 127; counter++) {
        OCR1B = counter;
        delay(15);
    }
}
