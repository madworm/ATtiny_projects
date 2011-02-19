#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#define __LATCH_LOW PORTB &= ~(1 << PB4)
#define __LATCH_HIGH PORTB |= (1 << PB4)
#define __DISPLAY_ON PORTB &= ~_BV(PB2)
#define __DISPLAY_OFF PORTB |= _BV(PB2)
#define __LED0_ON PORTB |= _BV(PB0)
#define __LED0_OFF PORTB &= ~_BV(PB0)
#define __LED1_ON PORTB |= _BV(PB1)
#define __LED1_OFF PORTB &= ~_BV(PB1)

#define __step_delay 1500 // used to let the amp-meter settle on a current value

#define __max_brightness 64 // higher numbers at your own risk - should be divisible by 8 ;-)
#define __pwm_loop_max __max_brightness - 1
#define __OCR1A ( 0x008 * (__max_brightness / 8) )
#define __fade_delay ( 1024 / (__max_brightness / 8) )

void setup(void);
void loop(void);
void no_isr_demo(void);
void fader(void);
void delay(uint32_t ticks);
void current_calib(void);
void setup_system_ticker(void);
uint32_t time(void);
void setup_timer1_ctc(void);
uint8_t spi_transfer(uint8_t data);
int main(void);

volatile uint8_t brightness[8] = {0,0,0,0,0,0,0,0};
volatile uint32_t system_ticks = 0;

int main(void) {
	setup();
	for(;;) {
		loop();
	}
};

void loop(void) {
  fader();
}

void setup(void) {
  DDRB |= _BV(PB0); // set LED pin as output
  __LED0_ON;

  DDRB |= _BV(PB1); // 2nd LED pin
  __LED1_ON;

  DDRB |= _BV(PB2); // display enable pin as output
  PORTB |= _BV(PB2); // pullup on
  
  // USI stuff
  
  DDRB |= _BV(PB6); // as output (DO)
  DDRB |= _BV(PB7); // as output (USISCK)
  DDRB &= ~_BV(PB5); // as input (DI)
  PORTB |= _BV(PB5); // pullup on (DI)

  sei(); // turn global irq flag on

  setup_system_ticker();
  setup_timer1_ctc();
  current_calib();
}

void no_isr_demo(void) {
  __DISPLAY_ON;
  __LATCH_LOW;
    spi_transfer(0x01); // ch1 on
  __LATCH_HIGH;
  delay(__step_delay);

  PORTB ^= _BV(PB0); // toggle LED

  __LATCH_LOW;
    spi_transfer(0x03); // ch1+2 on
  __LATCH_HIGH;
  delay(__step_delay);

  PORTB ^= _BV(PB0); // toggle LED

  __LATCH_LOW;
    spi_transfer(0x07); // ch1+2+3 on
  __LATCH_HIGH;
  delay(__step_delay);

  PORTB ^= _BV(PB0); // toggle LED

  __LATCH_LOW;
    spi_transfer(0x00); // all off
  __LATCH_HIGH;
  delay(__step_delay);

  PORTB ^= _BV(PB0); // toggle LED

  __LATCH_LOW;
    spi_transfer(0x00); // all outputs on
  __LATCH_HIGH;
  delay(__step_delay);

  PORTB ^= _BV(PB0); // toggle LED
  __DISPLAY_OFF;
}

void fader(void) {
  uint8_t ctr1;
  uint8_t ctr2;
  for(ctr1 = 0; ctr1 <= __max_brightness; ctr1++) {
    for(ctr2 = 0; ctr2 <= 7; ctr2++) {
      brightness[ctr2] = ctr1;
    }	
    delay(__fade_delay);
  }
  for(ctr1 = __max_brightness; (ctr1 >= 0) && (ctr1 != 255); ctr1--) {
    for(ctr2 = 0; ctr2 <= 7; ctr2++) {
      brightness[ctr2] = ctr1;
    }	
    delay(__fade_delay);
  }
}

void current_calib(void) {
  uint8_t ctr1;
  for(ctr1 = 0; ctr1 <= 7; ctr1++) {
    brightness[ctr1] = 255;
  }
  delay(50000);
}

uint32_t time(void)
{
  uint8_t _sreg = SREG;
  uint32_t time;
  cli();
  time = system_ticks;
  SREG = _sreg;
  return time;
}

void delay(uint32_t ticks)
{
  uint32_t start_time = time();
  while( (time() - start_time) < ticks ) {
    // just wait here
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

void setup_system_ticker(void)
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

void setup_timer1_ctc(void)
{
	uint8_t _sreg = SREG;	/* save SREG */
	cli();			/* disable all interrupts while messing with the register setup */

	/* multiplexed TRUE-RGB PWM mode (quite dim) */
	/* set prescaler to 8 */
	TCCR1B |= (_BV(CS11));
	TCCR1B &= ~(_BV(CS10) | _BV(CS12));
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

ISR(TIMER0_OVF_vect)
{
  __LED1_ON;
  system_ticks++;  
  __LED1_OFF;
}

ISR(TIMER1_COMPA_vect)
{				/* Framebuffer interrupt routine */
	static uint8_t pwm_cycle = 0;

	__DISPLAY_ON;		// only enable the drivers when we actually have time to talk to them
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
	__DISPLAY_OFF;		// we're done with this line, turn the driver's off until next time

}
