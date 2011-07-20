#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include <util/delay.h>
#include "main.h"
#include "system_ticker.h"
#include "spi.h"
#include "util.h"
#include "uart.h"
#include "adc.h"
#include "led_driver.h"

int main(void)
{
    setup_hw();
    delay(6000);
    for (;;) {
        //soft_uart_rx_test();
        //adc_test(1); // shows ADCH on the 8 LEDs. timer1 should be OFF (or it blinks like mad --> headache)
        TOGGLE_LED; // make the lamps visible in the darkness
        kitchen_lights(1);
        // this saved about 2mA on my dev board
        sleep_enable(); // make it possible to have some zzzzz-s
        sleep_cpu();    // good night
        sleep_disable(); // we've just woken up again
    }
};

void kitchen_lights(uint8_t channel)
{
    // 0: off or user has decreased brightness
    // 1: on or user has increased brightness
    static uint8_t lamp_state = 0;
    static uint8_t switches_state = 0;
    static uint8_t ctr = OCR1A_MAX;
    uint8_t rx_byte;

    uint8_t adc_tmp = adc_read(channel); // switches + resistor network connected to PA1

    /*
        board 1: tested with adc_test(1) and timer1 OFF !

        switches pressed    voltage     binary MSB...LSB     ADCH    state

        none                -.--        11111111            255     0
        1                   -.--        10000000            128     1
        2                   -.--        10101010            170     2
        3                   -.--        11001101            205     3
        1+2                 -.--        01100110            102     4
        3+2                 -.--        01001001            146     5


        board 2: tested with adc_test(1) and timer1 OFF !

        switches pressed    voltage     ADCH    state

        none                -.--        11111111            255     0
        1                   -.--        01111111            127     1
        2                   -.--        10101010            170     2
        3                   -.--        11001101            205     3
        1+2                 -.--        01100110            102     4
        3+2                 -.--        10010010            146     5


        board 3: tested with adc_test(1) and timer1 OFF !

        switches pressed    voltage     ADCH    state

        none                -.--        11111111            255     0
        1                   -.--        10000000            127     1
        2                   -.--        10101011            171     2
        3                   -.--        11001101            205     3
        1+2                 -.--        01100110            102     4
        3+2                 -.--        10010010            146     5
    */

    // evaluate ADCH and translate it to which buttons are pressed

    switches_state = 0; // reset

    if ( soft_uart_peek() ) {
        rx_byte = soft_uart_read(); // read byte and reset flag (see internals)
        // other board has signalled fade-in/out toggle
        switch(rx_byte) {
        case '+':
            switches_state = 3; // manual fade-in
            break;
        case '-':
            switches_state = 1; // manual fade-out
            break;
        case 'f':
            switches_state = 2; // toggle fade-in/out
            break;
        default:
            break;
        }

        /*
            // only for debugging
            delay(500); // wait before we echo it back
            soft_uart_send('\n');
            soft_uart_send('\r');
            soft_uart_send('e');
            soft_uart_send(':');
            soft_uart_send(' ');
            soft_uart_send(rx_byte); // echo what we 'think' we got
            soft_uart_send('\n');
            soft_uart_send('\r');
        */
    }

    // the buttons are checked after serial, so they can override (when held pressed)

    if ( adc_tmp > 122 && adc_tmp < 132 ) {
        switches_state = 1;
    }
    if ( adc_tmp > 165 && adc_tmp < 175 ) {
        switches_state = 2;
    }
    if ( adc_tmp > 200 && adc_tmp < 210 ) {
        switches_state = 3;
    }
    if (adc_tmp > 97 && adc_tmp < 107 ) {
        switches_state = 4;
    }
    if (adc_tmp > 141 && adc_tmp < 151) {
        switches_state = 5;
    }

    if ( switches_state == 1 ) { // reduce brightness
        lamp_state = 0;
        LED_OFF;

        if (ctr < OCR1A_MAX) {
            ctr = ctr + 5; // inversed logic, higher ctr is less brightness
        }

        brightness = ctr; // write to volatile
        delay(MANUAL_FADE_OUT_DELAY); // manually fading out
    }

    if ( switches_state == 3 ) { // increase brightness
        lamp_state = 1;
        LED_ON;

        if (ctr > 0) {
            ctr = ctr - 5; // inversed logic, smaller ctr is more brightness
        }

        brightness = ctr;
        delay(MANUAL_FADE_IN_DELAY); // manually fading in
    }

    if ( switches_state == 2 ) {
        if (ctr == OCR1A_MAX) {	// fully off
            lamp_state = 1;	// we increased brightness
            LED_ON;
            fade_in(ctr,AUTO_FADE_IN_DELAY); // auto-fade-in
            ctr = 0;
        } else if (ctr == 0) {	// fully on
            lamp_state = 0;	// we decreased brightness
            LED_OFF;
            fade_out(ctr,AUTO_FADE_OUT_DELAY); // auto-fade-out
            ctr = OCR1A_MAX;
        }

        if (lamp_state == 0) {	// user pressed "-"
            fade_out(ctr,MANUAL_FADE_OUT_DELAY);
            ctr = OCR1A_MAX;
        }
        if (lamp_state == 1) {	// user pressed "+"
            fade_in(ctr,MANUAL_FADE_IN_DELAY);
            ctr = 0;
        }
        delay(1500);	// until I have debounced buttons...
    }

    if ( switches_state == 4 ) { // remote fade-out
        uint16_t start_time = time();
        uint16_t elapsed_time = 0;
        while ( adc_read(channel) < 240  ) { // any button is still pressed, most likely the ones for this case
            // wait to create pulse of certain length
            LED_OFF;
            elapsed_time = time() - start_time;

            if ( elapsed_time > 5000 ) {
                soft_uart_send('-');  // decrease by one step
            }
        }
        if ( elapsed_time > 1500 && elapsed_time < 5000 ) {
            // just a short press
            soft_uart_send('f'); // toggle fade in/out
        }
    }

    if ( switches_state == 5 ) { // remote fade-in
        uint16_t start_time = time();
        uint16_t elapsed_time = 0;
        while ( adc_read(channel) < 240  ) { // any button is still pressed, most likely the ones for this case
            // wait to create pulse of certain length
            LED_OFF;
            elapsed_time = time() - start_time;

            if ( elapsed_time > 5000 ) {
                soft_uart_send('+');  // increase by one step
            }
        }
        if ( elapsed_time > 1500 && elapsed_time < 5000 ) {
            // just a short press
            soft_uart_send('f'); // toggle fade in/out
        }
    }

}

void setup_hw(void)
{
    cli();  // turn interrupts off, just in case

    /*
     * power savings !
     *
     * total optimized power consumption of the cpu module: 2.2mA
     * incl. status LED (PWMed)
     *
     * the MBI5168 LED drivers take about 15mA each in idle mode :-(
     *
     */

    // turn the watchdog off
    wdt_reset();
    MCUSR= 0x00;
    WDTCSR |= ( _BV(WDCE) | _BV(WDE) ); // timed sequence !
    WDTCSR = 0x00;

    // turn all pins to inputs + pull-up on
    // saved about another 0.5mA on my board
    DDRA = 0x00;
    DDRB = 0x00;
    PORTA = 0xFF;
    PORTB = 0xFF;

    /*
     * now configure the pins we actually need
     */
    DDRB |= _BV(PB0);	// display enable pin as output
    PORTB |= _BV(PB0);	// pullup on

    DDRB |= _BV(PB2);   // led pin as output
    PORTB |= _BV(PB2);  // led on

    // USI stuff
    DDRB |= _BV(PB1);   // latch pin as output

    DDRA |= _BV(PA5);	// as output (DO)
    DDRA |= _BV(PA4);	// as output (USISCK)
    DDRA &= ~_BV(PA6);	// as input (DI)
    PORTA |= _BV(PA6);	// pullup on (DI)

    // only for debugging
    DDRA |= _BV(PA2);   // as output
    PORTA &= _BV(PA2);  // set LOW
    DDRA |= _BV(PA3);   // as output
    PORTA &= _BV(PA3);  // set LOW
    DDRA |= _BV(PA7);   // as output
    PORTA &= _BV(PA7);  // set LOW

    /*
     * setup ADC
     *
     * using single conversion mode
     * --> setup in adc_read() for every conversion necessary!
     *
     */
    PORTA &= ~_BV(PA1); // internal pull-up off on switches pin

    // sleep mode
    // set_sleep_mode(SLEEP_MODE_IDLE);

    /*
     * getting ready
     */
    system_ticker_setup();
    led_driver_setup(); // disable for adc_test()
    soft_uart_setup();

    sei(); // turn global irq flag on
    signal_reset(); // needs the system_ticker to run and sei() as well !

    DISPLAY_OFF; // turn the driver off

    LATCH_LOW;
    spi_transfer(0xFF);	// set wich channels are active
    LATCH_HIGH;
}
