#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "system_ticker.h"
#include "spi.h"
#include "uart.h"
#include "button.h"
#include "led_driver.h"
#include "status_leds.h"
#include "main.h"


int main(void)
{
    setup_hw();
    delay(3000);
    for (;;) {
        //button_test(); // shows the button states on the 8 LEDs. timer1 should be OFF (or it blinks like mad --> headache)
        //uart_half_duplex_test();
        S_LED_TOGGLE; // make the lamps visible in the darkness
        kitchen_lights();
        // this saved about 2mA on my dev board
        sleep_enable(); // make it possible to have some zzzzz-s
        sleep_cpu();    // good night
        sleep_disable(); // we've just woken up again
    }
}

void kitchen_lights()
{
    if ( uart_avail() ) {
        switch( uart_read() ) {
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
    }

    SWITCHES_STATE_t switches_state = button_read_state();

    switch(switches_state) {
    case SW_RIGHT_PRESSED:
        eval_switch_state(SW_RIGHT_PRESSED,LJ_MANUAL_UP,LJ_FADE_IN);
        break;
    case SW_LEFT_PRESSED:
        eval_switch_state(SW_LEFT_PRESSED,LJ_MANUAL_DOWN,LJ_FADE_OUT);
        break;
    case SW_RIGHT_MIDDLE_PRESSED:
        eval_switch_state(SW_RIGHT_MIDDLE_PRESSED,LJ_SEND_REMOTE_UP,LJ_SEND_REMOTE_FADE_IN);
        break;
    case SW_LEFT_MIDDLE_PRESSED:
        eval_switch_state(SW_LEFT_MIDDLE_PRESSED,LJ_SEND_REMOTE_DOWN,LJ_SEND_REMOTE_FADE_OUT);
        break;
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

    // turn the watchdog off
    wdt_reset();
    MCUSR= 0x00;
    WDTCSR |= ( _BV(WDCE) | _BV(WDE) ); // timed sequence !
    WDTCSR = 0x00;

    // turn all pins to inputs + pull-up on
    // saved about another 0.5mA on my board
    DDRA = 0x00;
    DDRB = 0x00;
    DDRD = 0x00;
    PORTA = 0xFF;
    PORTB = 0xFF;
    PORTD = 0xFF;

    /*
     * now configure the pins we actually need
     */

    DDRD |= _BV(PD5); // make the S_LED pin an output

    /*
     * getting ready
     */
    set_sleep_mode(SLEEP_MODE_IDLE);
    system_ticker_setup();
    led_driver_setup();
    usi_setup();
    uart_setup();
    button_setup();

    sei(); // turn global irq flag on

    status_leds_test(); // needs the system_ticker to run and sei() as well !

    spi_transfer(0xFF);	// set wich channels are active
    spi_transfer(0xFF);	// set wich channels are active
    LATCH;
}

void process_lamp_job(LAMP_JOB_t job)
{
    switch(job) {
    case LJ_MANUAL_UP:
        up(MANUAL_UP_DELAY);
        break;
    case LJ_MANUAL_DOWN:
        down(MANUAL_DOWN_DELAY);
        break;
    case LJ_FADE_IN:
        fade_in(get_brightness(),AUTO_FADE_IN_DELAY);
        break;
    case LJ_FADE_OUT:
        fade_out(get_brightness(),AUTO_FADE_OUT_DELAY);
        break;
    case LJ_SEND_REMOTE_UP:
        uart_send('+');
        break;
    case LJ_SEND_REMOTE_DOWN:
        uart_send('-');
        break;
    case LJ_SEND_REMOTE_FADE_IN:
        uart_send('F');
        break;
    case LJ_SEND_REMOTE_FADE_OUT:
        uart_send('f');
        break;
    case LJ_RECVD_REMOTE_UP:
        up(MANUAL_UP_DELAY);
        break;
    case LJ_RECVD_REMOTE_DOWN:
        down(MANUAL_DOWN_DELAY);
        break;
    case LJ_RECVD_REMOTE_FADE_IN:
        fade_in(get_brightness(),AUTO_FADE_IN_DELAY);
        break;
    case LJ_RECVD_REMOTE_FADE_OUT:
        fade_out(get_brightness(),AUTO_FADE_OUT_DELAY);
        break;
    default:
        // LJ_NOP
        break;
    }
}

void eval_switch_state(SWITCHES_STATE_t state, LAMP_JOB_t first_job, LAMP_JOB_t second_job)
{
    uint16_t start_time = time();
    uint16_t elapsed_time = 0;
    while ( button_read_state() == state ) {
        elapsed_time = time() - start_time;

        if ( elapsed_time > 3000 ) {
            // long press
            process_lamp_job(first_job);
        }
    }
    if ( (elapsed_time > 1500) && (elapsed_time < 3000) ) {
        // not so long press
        process_lamp_job(second_job);
    }
    if ( (elapsed_time > 50) && (elapsed_time < 1000) ) {
        // short press
        process_lamp_job(first_job);
    }
}
