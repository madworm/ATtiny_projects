#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "adc.h"
#include "util.h"
#include "system_ticker.h"
#include "spi.h"
#include "led_driver.h"

void adc_test(uint8_t channel)
{
    // disable COMPA isr - prevents flickering
    TIMSK1 &= ~_BV(OCIE1A);

    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;
    delay(50);
    TOGGLE_LED;

    DISPLAY_OFF;
    LATCH_LOW;
    spi_transfer(0x01); // show LSB
    LATCH_HIGH;
    DISPLAY_ON;

    delay(200);

    while(1) {
        uint8_t tmp = adc_read(channel);

        DISPLAY_OFF;
        LATCH_LOW;
        spi_transfer(tmp); // show the data
        LATCH_HIGH;
        DISPLAY_ON;

        DISPLAY_OFF;
        LATCH_LOW;
        spi_transfer(0x00); // turn it off to make it less blindingly bright
        LATCH_HIGH;
        DISPLAY_ON;
    }
}

uint8_t adc_read(uint8_t channel)
{
    ADCSRA |= _BV(ADEN);    // enable ADC
    ADCSRA |= ( _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0) ); // set prescaler to 128 for stable readings
    ADCSRB |= _BV(ADLAR);   // set to left-aligned. we only need 8-bit and read ADCH only
    ADMUX &= ~( _BV(REFS1) | _BV(REFS0) );          // VREF = VCC
    ADMUX &= ~( _BV(MUX2) | _BV(MUX1) | _BV(MUX0) ); // reset to channel PA0

    ADMUX |= channel; // valid values for this board: 0,1,2,3,7
    ADCSRA |= _BV(ADSC);    // start conversion
    while( ADCSRA & _BV(ADSC) ) {
        // wait until ADC is done
    }
    return ADCH;
}

SWITCHES_STATE_t adc_read_state(uint8_t channel)
{
    uint8_t state = SW_ALL_OPEN;
    uint8_t adc_tmp;
    adc_tmp = adc_read(channel);

    if ( adc_tmp > 122 && adc_tmp < 132 ) {
        state = SW_LEFT_PRESSED;
     }
    if ( adc_tmp > 165 && adc_tmp < 175 ) {
        state = SW_MIDDLE_PRESSED;
     }
    if ( adc_tmp > 200 && adc_tmp < 210 ) {
        state = SW_RIGHT_PRESSED;
     }
    if (adc_tmp > 97 && adc_tmp < 107 ) {
        state = SW_LEFT_MIDDLE_PRESSED;
    }
    if (adc_tmp > 141 && adc_tmp < 151) {
        state = SW_RIGHT_MIDDLE_PRESSED;
    }
    return state;
}
