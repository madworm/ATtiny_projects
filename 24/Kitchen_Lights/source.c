#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include <util/delay.h>
#include "source.h"

// at 9600 one bit should take 0.104ms
#define HALF_BIT_DELAY 48  // tuned a bit after looking at the sample times
#define FULL_BIT_DELAY 100 // with my newly acquired OLS from Dangerous Prototypes ;-)

uint8_t brightness = __OCR1A_max; /* for all 8 channels. __OCR1A_max is fully off, 0 is fully on */

volatile uint16_t system_ticks = 0;
volatile uint8_t soft_uart_rx_byte = 0;
volatile uint8_t soft_uart_rx_flag = 0;

int main(void)
{
    setup_hw();
    delay(6000);
    for (;;) {
        loop();
        // this saved about 2mA on my dev board
        sleep_enable(); // make it possible to have some zzzzz-s
        sleep_cpu();    // good night
        sleep_disable(); // we've just woken up again
    }
};

void loop(void)
{
    //soft_uart_rx_test();
    //adc_test(1); // shows ADCH on the 8 LEDs. timer1 should be OFF (or it blinks like mad --> headache)
    //adjust_fade_delay(7); // read an LDR on PA7
    __TOGGLE_LED; // make the lamps visible in the darkness
    kitchen_lights(1);
}

void kitchen_lights(uint8_t channel)
{
    // re-enable pin-change interrupts on group 0
    GIMSK |= _BV(PCIE0);
    // this will be disabled as soon as the ISR runs
    // it is an attempt to prevent flooding of the
    // receiver, which upsets the LED driver ISR it
    // seems.

    // 0: off or user has decreased brightness
    // 1: on or user has increased brightness
    static uint8_t lamp_state = 0;
    static uint8_t switches_state = 0;
    static uint8_t ctr = __OCR1A_max;

    uint8_t adc_tmp = read_adc(channel); // switches + resistor network connected to PA1

    /*
        board 1: tested with adc_test(1) and timer1 OFF !

        switches pressed    voltage     binary MSB...LSB     ADCH    state

        none                -.--        11111111            255     0
        1                   -.--        10000000            128     1
        2                   -.--        10101010            170     2
        3                   -.--        11001101            205     3
        1+2                 -.--        01100110            102     4
        3+2                 -.--        01001001            146     5
    */

    /*
        board 2: tested with adc_test(1) and timer1 OFF !

        switches pressed    voltage     ADCH    state

        none                -.--        11111111            255     0
        1                   -.--        01111111            127     1
        2                   -.--        10101010            170     2
        3                   -.--        11001101            205     3
        1+2                 -.--        01100110            102     4
        3+2                 -.--        10010010            146     5
    */

    /*
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
    if ( soft_uart_rx_flag ) {
        // other board has signalled fade-in/out toggle
        switch(soft_uart_rx_byte) {
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
        soft_uart_rx_flag = 0; // clear the flag
        soft_uart_rx_byte = 0; // clear the data
    }

    if ( switches_state == 1 ) {
        lamp_state = 0;
        __LED_OFF;

        brightness = ctr;

        if (ctr < __OCR1A_max) {
            ctr = ctr + 5;
        }
        delay(2*__fade_delay); // manually fading out with fixed speed
    }

    if ( switches_state == 3 ) {
        lamp_state = 1;
        __LED_ON;

        brightness = ctr;

        if (ctr > 0) {
            ctr = ctr - 5;
        }
        delay(4*__fade_delay); // manually fading in with fixed speed, but slower ( better for the eyes in the morning ;-) )
    }

    if ( switches_state == 2 ) {
        if (ctr == __OCR1A_max) {	// fully off
            lamp_state = 1;	// we increased brightness
            __LED_ON;
            fade_in(ctr,__fade_delay); // auto-fadein adjusted by LDR
            ctr = 0;
        } else if (ctr == 0) {	// fully on
            lamp_state = 0;	// we decreased brightness
            __LED_OFF;
            fade_out(ctr,2*__fade_delay); // auto-fadeout with fixed speed
            ctr = __OCR1A_max;
        }

        if (lamp_state == 0) {	// user pressed "-"
            fade_out(ctr,2*__fade_delay);
            ctr = __OCR1A_max;
        }
        if (lamp_state == 1) {	// user pressed "+"
            fade_in(ctr,__fade_delay);
            ctr = 0;
        }
        delay(1500);	// until I have debounced buttons...
    }

    if ( switches_state == 4 ) { // remote fade-out
        uint16_t start_time = time();
        uint16_t elapsed_time = 0;
        while ( read_adc(channel) < 240  ) { // any button is still pressed, most likely the ones for this case
            // wait to create pulse of certain length
            __LED_OFF;
            elapsed_time = time() - start_time;

            if ( elapsed_time > 5000 ) {
                soft_uart_tx('-');  // decrease by one step
            }
        }
        if ( elapsed_time > 1500 && elapsed_time < 5000 ) {
            // just a short press
            soft_uart_tx('f'); // toggle fade in/out
        }
    }

    if ( switches_state == 5 ) { // remote fade-in
        uint16_t start_time = time();
        uint16_t elapsed_time = 0;
        while ( read_adc(channel) < 240  ) { // any button is still pressed, most likely the ones for this case
            // wait to create pulse of certain length
            __LED_OFF;
            elapsed_time = time() - start_time;

            if ( elapsed_time > 5000 ) {
                soft_uart_tx('+');  // increase by one step
            }
        }
        if ( elapsed_time > 1500 && elapsed_time < 5000 ) {
            // just a short press
            soft_uart_tx('f'); // toggle fade in/out
        }
    }

}

inline void setup_hw(void)
{
    cli();              // turn interrupts off

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
    //DDRA |= _BV(PA2);   // as output
    //PORTA &= _BV(PA2);  // set LOW
    //DDRA |= _BV(PA3);   // as output
    //PORTA &= _BV(PA3);  // set LOW
    //DDRA |= _BV(PA7);   // as output
    //PORTA &= _BV(PA7);  // set LOW

    /*
     * setup ADC
     *
     * using single conversion mode
     * --> setup in read_adc() for every conversion necessary!
     *
     */
    PORTA &= ~_BV(PA1); // internal pull-up off on switches pin

    // sleep mode
    // set_sleep_mode(SLEEP_MODE_IDLE);

    /*
     * getting ready
     */
    sei(); // turn global irq flag on
    setup_system_ticker();
    signal_reset();
    setup_timer1_ctc(); // disable for adc_test()
    setup_soft_uart_rx_isr();

    __DISPLAY_OFF; // turn the driver off

    __LATCH_LOW;
    spi_transfer(0xFF);	// set wich channels are active
    __LATCH_HIGH;
}

void fade_in(uint8_t start_at, uint16_t fade_delay)
{
    uint8_t ctr1;
    for (ctr1 = start_at; (ctr1 > 0); ctr1--) {
        brightness = ctr1;
        delay(fade_delay);
    }
}

void fade_out(uint8_t start_at, uint16_t fade_delay)
{
    uint8_t ctr1;
    for (ctr1 = start_at; ctr1 <= __OCR1A_max; ctr1++) {
        brightness = ctr1;
        delay(fade_delay);
    }
}

uint16_t time(void)
{
    uint8_t _sreg = SREG;
    uint16_t time;
    cli();
    time = system_ticks;
    SREG = _sreg;
    return time;
}

void delay(uint16_t ticks)
{
    uint16_t start_time = time();
    while ((time() - start_time) < ticks) {
        // just wait here
    }
}

void signal_reset(void)
{
    __TOGGLE_LED;
    delay(1000);
    __TOGGLE_LED;
    delay(1000);
    __TOGGLE_LED;
    delay(1000);
    __TOGGLE_LED;
    delay(1000);
}

/*
Functions dealing with hardware specific jobs / settings
*/

inline uint8_t spi_transfer(uint8_t data)
{
    USIDR = data;
    USISR = _BV(USIOIF);	// clear flag

    while ((USISR & _BV(USIOIF)) == 0) {
        USICR = (1 << USIWM0) | (1 << USICS1) | (1 << USICLK) | (1 << USITC);
    }
    return USIDR;
}

inline void setup_system_ticker(void)
{
    /* save SREG and turn interrupts off globally */
    uint8_t _sreg = SREG;
    cli();
    /* using timer0 */
    /* setting prescaler to 8 */
    TCCR0B |= _BV(CS01);
    TCCR0B &= ~(_BV(CS00) | _BV(CS02));
    /* set WGM mode 0 */
    TCCR0A &= ~(_BV(WGM01) | _BV(WGM00));
    TCCR0B &= ~_BV(WGM02);
    /* normal operation - disconnect PWM pins */
    TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0));
    /* enabling overflow interrupt */
    TIMSK0 |= _BV(TOIE0);
    /* restore SREG */
    SREG = _sreg;
}

inline void setup_timer1_ctc(void)
{
    uint8_t _sreg = SREG; /* save SREG */
    cli(); /* disable all interrupts while messing with the register setup */

    /* set prescaler to 256 */
    TCCR1B |= (_BV(CS12));
    TCCR1B &= ~(_BV(CS11) | _BV(CS10));
    /* set WGM mode 4: CTC using OCR1A */
    TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));
    TCCR1B |= _BV(WGM12);
    TCCR1B &= ~_BV(WGM13);
    /* normal operation - disconnect PWM pins */
    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));
    /* set top value for TCNT1 */
    OCR1A = __OCR1A_max;
    /* enable COMPA isr */
    TIMSK1 |= _BV(OCIE1A);
    /* restore SREG with global interrupt flag */
    SREG = _sreg;
}

ISR(TIM0_OVF_vect) /* on attiny2313/4313 this is named TIMER0_OVF_vect */
{
    system_ticks++;
}

ISR(TIM1_COMPA_vect) /* on attiny2313/4313 this is named TIMER1_COMPA_vect */
{
    // for debugging only
    //__PA2_ON;

    /* Framebuffer interrupt routine */
    __DISPLAY_OFF;

    /*
     * init as off and accept that __OCR1A_max = 0% and 0 = 100%
     * if inited as on, it won't go down to zero brightness
     * which is more annoying than not getting to 100%
     *
     */
    static uint8_t index = 0;
    static uint16_t OCR1A_TMP = 0;

    /* starts with index = 0 */
    /* now calculate when to run the next time and turn on all 8-ch */

    switch(index) {
    case 0:
        OCR1A_TMP = brightness;
        index++;
        break;
    case 1:
        if ( brightness == __OCR1A_max) {
            /* if the leds should be at 0%, do nothing */
        } else {
            __DISPLAY_ON; // turn all 8-ch on
        }
        /* calculate when to turn everything off */
        OCR1A_TMP = ( __OCR1A_max - brightness );
        index++;
        break;
    case 2:
        /* cycle completed, reset everything */
        index = 0;
        /* immediately restart */
        OCR1A_TMP = 0;
        /* DON'T increase the index counter ! */
        break;
    default:
        break;
    }

    OCR1A = OCR1A_TMP;

    // for debugging only
    //__PA2_OFF;
}

ISR(PCINT0_vect,ISR_NOBLOCK) // pin-change interrupt group 0
{
    /*
     * It turns out that adding this ISR delayed the
     * next execution of 'TIM1_COMPA_vect' long enough,
     * that setting the OCR1A register came too late
     * and the desired effect was delayed until TIMER1
     * had wrapped around completely (about 2s) :-(
     * by making this ISR interruptible, it is solved;
     * for this constellation at least.
     */

    // for debugging only
    //__PA3_ON;

    // disable pin-change interrupts on group 0 - ASAP !
    // it should be re-enabled elsewhere, after the data
    // has been processed
    GIMSK &= ~_BV(PCIE0);
    // soft UART receiver

    uint8_t ctr;

    if( !(PINA & _BV(PA0)) ) { // PA0 is low, start-bit
        // beginning of start-bit
        _delay_us(HALF_BIT_DELAY);
        // center of start-bit

        // for debugging only
        //__PA7_ON;
        //__PA7_OFF;

        _delay_us(HALF_BIT_DELAY);
        // end of start-bit
        _delay_us(HALF_BIT_DELAY);
        // center of data-bit 0
        for(ctr=0; ctr<=7; ctr++) { // receive byte LSB first

            // for debugging only
            //__PA7_ON;
            //__PA7_OFF;

            if( PINA & _BV(PA0) ) { // bit is set
                soft_uart_rx_byte |= _BV(ctr); // enter a 1
            } else { // bit is not set
                soft_uart_rx_byte &= ~_BV(ctr); // enter a 0
            }
            _delay_us(FULL_BIT_DELAY);
        }
        // now we should be at the center of the stop bit

        // for debugging only
        //__PA7_ON;
        //__PA7_OFF;

        if ( PINA & _BV(PA0) ) { //stop bit is HIGH as it should be
            soft_uart_rx_flag = 1;
        } else {
            soft_uart_rx_flag = 0;
        }
    }

    // for debugging only
    //__PA3_OFF;
}

void setup_soft_uart_rx_isr(void)
{
    PCMSK0 |= _BV(PCINT0); // select pin-change interrupt on PA0
    GIMSK |= _BV(PCIE0); // enable pin-change interrupts on group 0
}

void soft_uart_tx(uint8_t byte)
{
    GIMSK &= ~_BV(PCIE0); // disable pin-change interrupts on group 0

    uint8_t ctr;

    DDRA |= _BV(PA0); // make it an output
    PORTA &= ~_BV(PA0); // pull it low: start-bit
    _delay_us(FULL_BIT_DELAY);
    for(ctr=0; ctr<=7; ctr++) {
        if( (byte & _BV(ctr)) ) { // sent byte LSB first
            PORTA |= _BV(PA0);  // set pin
        } else {
            PORTA &= ~_BV(PA0); // clear pin
        }
        _delay_us(FULL_BIT_DELAY);
    }
    PORTA |= _BV(PA0);  // pull it high: stop-bit
    DDRA &= ~_BV(PA0);  // make it an input again

    _delay_us(10*FULL_BIT_DELAY); // don't flood the receiver
    _delay_us(10*FULL_BIT_DELAY); // a soft-uart as well

    GIMSK |= _BV(PCIE0);    // re-enable pin-change interrupts on group 0
}

void soft_uart_rx_test(void)
{
    /* disable COMPA isr - prevents flickering */
    TIMSK1 &= ~_BV(OCIE1A);

    __TOGGLE_LED;
    delay(50);
    __TOGGLE_LED;
    delay(50);
    __TOGGLE_LED;
    delay(50);
    __TOGGLE_LED;

    __DISPLAY_OFF;
    __LATCH_LOW;
    spi_transfer(0x01); // show LSB
    __LATCH_HIGH;
    __DISPLAY_ON;

    delay(200);

    while(1) {
        __DISPLAY_OFF;
        __LATCH_LOW;
        spi_transfer(soft_uart_rx_byte); // show the data
        __LATCH_HIGH;
        __DISPLAY_ON;

        __DISPLAY_OFF;
        __LATCH_LOW;
        spi_transfer(0x00); // turn it off to make it less blindingly bright
        __LATCH_HIGH;
        __DISPLAY_ON;
    }
}

void adc_test(uint8_t channel)
{
    /* disable COMPA isr - prevents flickering */
    TIMSK1 &= ~_BV(OCIE1A);

    __TOGGLE_LED;
    delay(50);
    __TOGGLE_LED;
    delay(50);
    __TOGGLE_LED;
    delay(50);
    __TOGGLE_LED;

    __DISPLAY_OFF;
    __LATCH_LOW;
    spi_transfer(0x01); // show LSB
    __LATCH_HIGH;
    __DISPLAY_ON;

    delay(200);

    while(1) {
        uint8_t tmp = read_adc(channel);

        __DISPLAY_OFF;
        __LATCH_LOW;
        spi_transfer(tmp); // show the data
        __LATCH_HIGH;
        __DISPLAY_ON;

        __DISPLAY_OFF;
        __LATCH_LOW;
        spi_transfer(0x00); // turn it off to make it less blindingly bright
        __LATCH_HIGH;
        __DISPLAY_ON;
    }
}

uint8_t read_adc(uint8_t channel)
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
