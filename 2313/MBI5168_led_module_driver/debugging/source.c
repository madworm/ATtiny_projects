// moved to makefile // #define F_CPU 8000000UL
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#define  __HAS_DELAY_CYCLES 0 

#define __LATCH_LOW PORTB &= ~(1 << PB4)
#define __LATCH_HIGH PORTB |= (1 << PB4)
#define __DISPLAY_ON PORTB &= ~_BV(PB1)
#define __DISPLAY_OFF PORTB |= _BV(PB1)

#define __max_brightness 16 // higher numbers at your own risk - should be divisible by 8 ;-)
#define __pwm_loop_max __max_brightness - 1
#define __OCR1A ( 0x0050 * (__max_brightness / 8) )

void setup(void);
void __delay_ms(uint16_t ms);
void current_calib(void);
void setup_timer1_ctc(void);
uint8_t spi_transfer(uint8_t data);
int main(void);

volatile uint8_t brightness[8] = {0,0,0,0,0,0,0,0};

int main(void) {
	setup();
	cli();
	__DISPLAY_OFF;
	__delay_ms(1000);
	PORTB |= _BV(PB0); // debug led on
	__delay_ms(1000);
	PORTB &= ~_BV(PB0); // off again
	for(;;) {} // stop here
};

void setup(void) {
  DDRB |= _BV(PB0); // set LED pin as output
  DDRB |= _BV(PB1); // display enable pin as output
  PORTB |= _BV(PB1); // pullup on
  
  // USI stuff
  
  DDRB |= _BV(PB6); // as output (DO)
  DDRB |= _BV(PB7); // as output (USISCK)
  DDRB &= ~_BV(PB5); // as input (DI)
  PORTB |= _BV(PB5); // pullup on (DI)

  sei(); // turn global irq flag on

  setup_timer1_ctc();
  current_calib();
}

void current_calib(void) {
  uint8_t ctr1;
  for(ctr1 = 0; ctr1 <= 7; ctr1++) {
    brightness[ctr1] = 255;
  }
  PORTB |= _BV(PB0);
  __delay_ms(1000);
  PORTB &= ~_BV(PB0);
}

void __delay_ms(uint16_t ms) {
  uint16_t ctr1;
  for(ctr1 = 0; ctr1 < ms; ctr1++ ){
    _delay_ms(1);	  
  }
}

/*
Functions dealing with hardware specific jobs / settings
*/

uint8_t spi_transfer(uint8_t data) {
  USIDR = data;
  USISR = _BV(USIOIF); // clear flag

  while ( (USISR & _BV(USIOIF)) == 0 ) {
   USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);
  }
  return USIDR;
}

void setup_timer1_ctc(void)
{
	uint8_t _sreg = SREG;	/* save SREG */
	cli();			/* disable all interrupts while messing with the register setup */

	/* multiplexed TRUE-RGB PWM mode (quite dim) */
	/* set prescaler to 1 */
	TCCR1B |= (_BV(CS10));
	TCCR1B &= ~(_BV(CS11) | _BV(CS12));
	/* set WGM mode 4: CTC using OCR1A */
	TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));
	TCCR1B |= _BV(WGM12);
	TCCR1B &= ~_BV(WGM13);
	/* normal operation - disconnect PWM pins */
	TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));
	/* set top value for TCNT1 */
	OCR1A = __OCR1A;
	/* enable COMPA isr */
	TIMSK |= _BV(OCIE1A);
	/* restore SREG with global interrupt flag */
	SREG = _sreg;
}

ISR(TIMER1_COMPA_vect)
{				/* Framebuffer interrupt routine */

	__DISPLAY_ON;

	static uint8_t pwm_cycle = 0;

		uint8_t led;
		uint8_t red = 0x00;	// off

		for (led = 0; led <= 7; led++) {
			if ( pwm_cycle < brightness[led] ) {
				red |= _BV(led);
			}
			else {
				red &= ~_BV(led);
			}
		}

		__LATCH_LOW;
		spi_transfer(red);
		__LATCH_HIGH;

        pwm_cycle++;

        if(pwm_cycle > __pwm_loop_max) {
	  pwm_cycle = 0;
	}	  

	__DISPLAY_OFF;	
}
