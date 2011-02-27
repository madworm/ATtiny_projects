#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "source.h"

// the 2nd number is actually 1-dutycycle.
// this array MUST be sorted ascendingly with respect to the 2nd number
// or the framebuffer ISR will mess up.
// the 2nd number determines when the signal line is supposed to go UP
// and all of these must be staggered.
// the array is split into 2 for double buffering
led_t glob_brightness[2][8] = { { {0, 0}, {1,128}, {2, 0}, {3, 128}, {4, 0}, {5, 128}, {6, 0}, {7, 128} },
    			        { {0, 0}, {1,128}, {2, 0}, {3, 128}, {4, 0}, {5, 128}, {6, 0}, {7, 128} } };

// array of pointers, pointing to glob_brightness elements such that it appears sorted (keeping the original data untouched)!
// initially unsorted, need bubbleSort at least once and for every order breaking manipulation to glob_brightness
led_t * sorted[2][8]; 
volatile uint8_t read_from_this = 0;
volatile uint8_t write_to_this = 1;
volatile uint8_t flip_req = 0;

volatile uint32_t system_ticks = 0;

int main(void)
{
	populate_sorted();
	bubbleSort(&sorted[read_from_this][0],8);
	bubbleSort(&sorted[write_to_this][0],8);
	setup_hw();
	for (;;) {
		loop();
	}
};

static inline void loop(void)
{
	fader();
	
	glob_brightness[write_to_this][4].dutycycle = 128;
	bubbleSort(&sorted[write_to_this][0],8);
	flip_req = 1;
	delay(10000);

	glob_brightness[write_to_this][4].dutycycle = 0;
	bubbleSort(&sorted[write_to_this][0],8);
	flip_req = 1;
	delay(10000);
}

static inline void setup_hw(void)
{
	DDRB |= _BV(PB0);	// set LED pin as output
	__LED0_ON;

	DDRB |= _BV(PB1);	// 2nd LED pin
	__LED1_ON;

	DDRB |= _BV(PB2);	// display enable pin as output
	PORTB |= _BV(PB2);	// pullup on

	// USI stuff

	DDRB |= _BV(PB6);	// as output (DO)
	DDRB |= _BV(PB7);	// as output (USISCK)
	DDRB &= ~_BV(PB5);	// as input (DI)
	PORTB |= _BV(PB5);	// pullup on (DI)

	sei();			// turn global irq flag on

	setup_system_ticker();
	setup_timer1_ctc();
	//current_calib();
	__DISPLAY_ON;
}

static void no_isr_demo(void)
{
	__DISPLAY_ON;
	__LATCH_LOW;
	spi_transfer(0x01);	// ch1 on
	__LATCH_HIGH;
	delay(__step_delay);

	PORTB ^= _BV(PB0);	// toggle LED

	__LATCH_LOW;
	spi_transfer(0x03);	// ch1+2 on
	__LATCH_HIGH;
	delay(__step_delay);

	PORTB ^= _BV(PB0);	// toggle LED

	__LATCH_LOW;
	spi_transfer(0x07);	// ch1+2+3 on
	__LATCH_HIGH;
	delay(__step_delay);

	PORTB ^= _BV(PB0);	// toggle LED

	__LATCH_LOW;
	spi_transfer(0x00);	// all off
	__LATCH_HIGH;
	delay(__step_delay);

	PORTB ^= _BV(PB0);	// toggle LED

	__LATCH_LOW;
	spi_transfer(0x00);	// all outputs on
	__LATCH_HIGH;
	delay(__step_delay);

	PORTB ^= _BV(PB0);	// toggle LED
	__DISPLAY_OFF;
}

static void fader(void)
{
	uint8_t ctr1;
	uint8_t ctr2;
	for (ctr1 = 0; ctr1 <= 128; ctr1++) {
		for (ctr2 = 0; ctr2 <= 7; ctr2++) {
			glob_brightness[write_to_this][ctr2].dutycycle = ctr1;
			bubbleSort(&sorted[write_to_this][0],8);
			flip_req = 1;
		}
		delay(__fade_delay);
	}
	for (ctr1 = 128; (ctr1 >= 0) && (ctr1 != 255); ctr1--) {
		for (ctr2 = 0; ctr2 <= 7; ctr2++) {
			glob_brightness[write_to_this][ctr2].dutycycle = ctr1;
			bubbleSort(&sorted[write_to_this][0],8);
			flip_req = 1;
		}
		delay(__fade_delay);
	}
}

static inline void current_calib(void)
{
	uint8_t ctr1;
	for (ctr1 = 0; ctr1 <= 7; ctr1++) {
		// brightness[ctr1] = 255;
	}
	delay(50000);
}

static inline uint32_t time(void)
{
	uint8_t _sreg = SREG;
	uint32_t time;
	cli();
	time = system_ticks;
	SREG = _sreg;
	return time;
}

static inline void delay(uint32_t ticks)
{
	uint32_t start_time = time();
	while ((time() - start_time) < ticks) {
		// just wait here
	}
}

static inline void populate_sorted(void)
{
	uint8_t index;
	for(index = 0; index < 8; index++)
	{
		sorted[0][index] = &glob_brightness[0][index];
		sorted[1][index] = &glob_brightness[1][index];
	}
}

static void bubbleSort(led_t ** array, uint8_t length)
{
	uint8_t i;
	uint8_t j;
	led_t * temp;
	uint8_t test;

	for(i = (length - 1); i > 0; i--)
	{
		test = 0;
		for(j = 0; j < i; j++)
		{
			if( (*array[j]).dutycycle > (*array[j+1]).dutycycle )
			{
				temp = array[j];
				array[j] = array[j+1];
				array[j+1] = temp;
				test = 1;
			}
		}
		if(test == 0)
		{
			break;
		}
	}
}

static inline void flip_buffers()
{
	uint8_t temp;
	temp = read_from_this;
	read_from_this = write_to_this;
	write_to_this = temp;
}

/*
Functions dealing with hardware specific jobs / settings
*/

static inline uint8_t spi_transfer(uint8_t data)
{
	USIDR = data;
	USISR = _BV(USIOIF);	// clear flag

	while ((USISR & _BV(USIOIF)) == 0) {
		USICR =
		    (1 << USIWM0) | (1 << USICS1) | (1 << USICLK) | (1 <<
								     USITC);
	}
	return USIDR;
}

static inline void setup_system_ticker(void)
{
	/* save SREG and turn interrupts off globally */
	uint8_t _sreg = SREG;
	cli();
	/* using timer0 */
	/* setting prescaler to 1 */
	TCCR0B |= _BV(CS00);
	TCCR0B &= ~(_BV(CS01) | _BV(CS02));
	/* set WGM mode 0 */
	TCCR0A &= ~(_BV(WGM01) | _BV(WGM00));
	TCCR0B &= ~_BV(WGM02);
	/* normal operation - disconnect PWM pins */
	TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0));
	/* enabling overflow interrupt */
	TIMSK |= _BV(TOIE0);
	/* restore SREG */
	SREG = _sreg;
}

static inline void setup_timer1_ctc(void)
{
	uint8_t _sreg = SREG;	/* save SREG */
	cli();			/* disable all interrupts while messing with the register setup */

	/* multiplexed TRUE-RGB PWM mode (quite dim) */
	/* set prescaler to 1024 */
	TCCR1B |= (_BV(CS10) | _BV(CS12));
	TCCR1B &= ~(_BV(CS11));
	/* set WGM mode 4: CTC using OCR1A */
	TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));
	TCCR1B |= _BV(WGM12);
	TCCR1B &= ~_BV(WGM13);
	/* normal operation - disconnect PWM pins */
	TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));
	/* set top value for TCNT1 */
	OCR1A = __OCR1A_max;
	/* enable COMPA isr */
	TIMSK |= _BV(OCIE1A);
	/* restore SREG with global interrupt flag */
	SREG = _sreg;
}

ISR(TIMER0_OVF_vect)
{
	__LED1_ON;
	system_ticks++;
	__LED1_OFF;
}

ISR(TIMER1_COMPA_vect)
{				/* Framebuffer interrupt routine */
	__LED0_ON;
	__DISPLAY_OFF;
	static uint8_t data = 0;	// init as off
	static uint8_t index = 0;

	/* starts with index = 0 */
	/* now calculate when to run the next time and turn on LED0 */
	if (index == 0) {
		if (flip_req == 1)
		{
			flip_buffers(); // only do this at the beginning of a NEW cycle !
			flip_req = 0;
		}
		OCR1A =
		    (uint16_t) ( (*sorted[read_from_this][index]).dutycycle );
		index++;
	} else if (index == 8) {	// the last led in the row
		data |= _BV( (*sorted[read_from_this][(index - 1)]).number );
		/* calculate when to turn everything off */
		OCR1A =
		    (uint16_t) ( 128 - (*sorted[read_from_this][(index - 1)]).dutycycle );
		index++;
	} else if (index == 9) {
		/* cycle completed, reset everything */
		data = 0;
		index = 0;
		/* immediately restart */
		OCR1A = 0;
		/* DON'T increase the index counter ! */
	} else {
		/* turn on the LED we deciced to turn on in the last invocation */
		data |= _BV( (*sorted[read_from_this][(index - 1)]).number );
		/* calculate when to run the next time and turn on the next LED */
		OCR1A =
		    (uint16_t) ( (*sorted[read_from_this][index]).dutycycle - (*sorted[read_from_this][(index - 1)]).dutycycle );
		index++;
	}

	__LATCH_LOW;
	spi_transfer(data);
	__LATCH_HIGH;

	__DISPLAY_ON;
	__LED0_OFF;
}
