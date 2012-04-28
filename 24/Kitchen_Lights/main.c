#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>
#include "system_ticker.h"
#include "spi.h"
#include "util.h"
#include "uart.h"
#include "adc.h"
#include "led_driver.h"
#include "main.h"

//#define DEBUG
#define USE_SOFT_UART

int main(void)
{
    setup_hw();
    delay(6000);
    for (;;) {
        //soft_uart_rx_test();
        //adc_test(1); // shows ADCH on the 8 LEDs. timer1 should be OFF (or it blinks like mad --> headache)
        TOGGLE_LED; // make the lamps visible in the darkness
        kitchen_lights(1);
    }
}

void kitchen_lights(uint8_t channel)
{
    #ifdef USE_SOFT_UART
    uint8_t rx_byte;

    if ( soft_uart_peek() ) {
        rx_byte = soft_uart_read(); // read byte and reset flag (see internals)
        switch(rx_byte) {
        case '+':
            process_lamp_job(LJ_RECVD_REMOTE_UP);
            break;
        case '-':
            process_lamp_job(LJ_RECVD_REMOTE_DOWN);
            break;
        case 'F':
            process_lamp_job(LJ_RECVD_REMOTE_FADE_IN);
            break;
        case 'f':
            process_lamp_job(LJ_RECVD_REMOTE_FADE_OUT);
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
    #endif

    SWITCHES_STATE_t switches_state = adc_read_state(1);

    if( my_mcusr & _BV(PORF) ) { // automatically fade in to full brightness
        my_mcusr = 0;            // if the lamp is supplied with power
        process_lamp_job(LJ_FADE_IN);
    }

    switch(switches_state) {
    case SW_RIGHT_PRESSED:
        eval_switch_state(SW_RIGHT_PRESSED,LJ_MANUAL_UP,LJ_FADE_IN);
        break;
    case SW_LEFT_PRESSED:
        eval_switch_state(SW_LEFT_PRESSED,LJ_MANUAL_DOWN,LJ_FADE_OUT);
        break;
    #ifdef USE_SOFT_UART
    case SW_RIGHT_MIDDLE_PRESSED:
        eval_switch_state(SW_RIGHT_MIDDLE_PRESSED,LJ_SEND_REMOTE_UP,LJ_SEND_REMOTE_FADE_IN);
        break;
    case SW_LEFT_MIDDLE_PRESSED:
        eval_switch_state(SW_LEFT_MIDDLE_PRESSED,LJ_SEND_REMOTE_DOWN,LJ_SEND_REMOTE_FADE_OUT);
        break;
    #endif
    default:
        // SW_ALL_OPEN
        break;
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

    /*
     * turn the watchdog off
     *
     * This should be off by default anyway I think
     * I didn't burn the WD fuse
     *
     */

    // wdt_reset();
    // MCUSR= 0x00;
    // WDTCSR |= ( _BV(WDCE) | _BV(WDE) ); // timed sequence !
    // WDTCSR = 0x00;

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
    PORTB &= ~_BV(PB1);	// latch low

    DDRA |= _BV(PA5);	// as output (DO)
    DDRA |= _BV(PA4);	// as output (USISCK)
    DDRA &= ~_BV(PA6);	// as input (DI)
    PORTA |= _BV(PA6);	// pullup on (DI)

    // only for debugging
    // DDRA |= _BV(PA2);   // as output
    // PORTA &= _BV(PA2);  // set LOW
    // DDRA |= _BV(PA3);   // as output
    // PORTA &= _BV(PA3);  // set LOW
    // DDRA |= _BV(PA7);   // as output
    // PORTA &= _BV(PA7);  // set LOW

    /*
     * setup ADC
     *
     * using single conversion mode
     * --> setup in adc_read() for every conversion necessary!
     *
     */
    PORTA &= ~_BV(PA1); // internal pull-up off on switches pin

    /*
     * getting ready
     */
    system_ticker_setup();
    led_driver_setup(); // disable for adc_test()
    #ifdef USE_SOFT_UART
    soft_uart_setup();
    #endif

    sei(); // turn global irq flag on
    #ifdef DEBUG
    signal_reset(); // needs the system_ticker to run and sei() as well !
    #endif
}

void process_lamp_job(LAMP_JOB_t job)
{
    int16_t tmp;
    int16_t tmp2;

    switch(job) {
    case LJ_MANUAL_UP:
        up(MANUAL_UP_DELAY);
        break;
    case LJ_MANUAL_DOWN:
        down(MANUAL_DOWN_DELAY);
        break;
    case LJ_FADE_IN:
        tmp = get_brightness();
        tmp2 = (tmp<LAMP_BRIGHTNESS_MAX/2)?LAMP_BRIGHTNESS_MAX/2:LAMP_BRIGHTNESS_MAX;
        fade(tmp,tmp2,AUTO_FADE_IN_DELAY);
        break;
    case LJ_FADE_OUT:
        tmp = get_brightness();
        tmp2 = (tmp>LAMP_BRIGHTNESS_MAX/2)?LAMP_BRIGHTNESS_MAX/2:0;
        fade(tmp,tmp2,AUTO_FADE_IN_DELAY);
        break;
#ifdef USE_SOFT_UART
    case LJ_SEND_REMOTE_UP:
        soft_uart_send('+');
        break;
    case LJ_SEND_REMOTE_DOWN:
        soft_uart_send('-');
        break;
    case LJ_SEND_REMOTE_FADE_IN:
        soft_uart_send('F');
        break;
    case LJ_SEND_REMOTE_FADE_OUT:
        soft_uart_send('f');
        break;
    case LJ_RECVD_REMOTE_UP:
        up(MANUAL_UP_DELAY);
        break;
    case LJ_RECVD_REMOTE_DOWN:
        down(MANUAL_DOWN_DELAY);
        break;
    case LJ_RECVD_REMOTE_FADE_IN:
        tmp = get_brightness();
        tmp2 = (tmp<LAMP_BRIGHTNESS_MAX/2)?LAMP_BRIGHTNESS_MAX/2:LAMP_BRIGHTNESS_MAX;
        fade(tmp,tmp2,AUTO_FADE_IN_DELAY);
        break;
    case LJ_RECVD_REMOTE_FADE_OUT:
        tmp = get_brightness();
        tmp2 = (tmp>LAMP_BRIGHTNESS_MAX/2)?LAMP_BRIGHTNESS_MAX/2:0;
        fade(tmp,tmp2,AUTO_FADE_IN_DELAY);
        break;
#endif
    default:
        // LJ_NOP
        break;
    }
}

void eval_switch_state(SWITCHES_STATE_t state, LAMP_JOB_t first_job, LAMP_JOB_t second_job)
{
    uint16_t start_time = time();
    uint16_t elapsed_time = 0;
    #ifdef PATTERN_A
    uint8_t already_flashed = 0;
    #endif

    while ( adc_read_state(1) == state ) {
        elapsed_time = time() - start_time;

        #ifdef PATTERN_A
        if ( (elapsed_time > 1500) && (already_flashed == 0) ) {
            //start_time += flash_status_led(5,500);
            start_time += flash_channel(0,5,500);
            already_flashed = 1;
        }
        #endif

        #ifdef PATTERN_B
            // not enough FLASH to use the above
        #endif

        if ( elapsed_time > 3000 ) {
            // long press
            process_lamp_job(first_job);
        }
    }
    if ( (elapsed_time > 1500) && (elapsed_time < 3000) ) {
        // just a short press
        process_lamp_job(second_job);
    }
    if ( (elapsed_time > 50) && (elapsed_time < 1000) ) {
        // short press
        process_lamp_job(first_job);
    }
}

void read_mcusr(void)
{
    my_mcusr = MCUSR;
    MCUSR = 0;
}
