#define F_CPU 8000000UL
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define __LATCH_LOW PORTB &= ~(1 << PB4)
#define __LATCH_HIGH PORTB |= (1 << PB4)

void setup(void);
void loop(void);
uint8_t spi_transfer(uint8_t data);
int main(void);

void setup(void) {
  DDRB |= _BV(PB0); // set LED pin as output
  PORTB |= _BV(PB0); // turn the LED on
  
  // USI stuff
  
  DDRB |= _BV(PB6); // as output (DO)
  DDRB |= _BV(PB7); // as output (USISCK)
  DDRB &= ~_BV(PB5); // as input (DI)
  PORTB |= _BV(PB5); // pullup on (DI)
}

void loop(void) {
  __LATCH_LOW;
    spi_transfer(0x01); // all outputs on
  __LATCH_HIGH;
  _delay_ms(500);

  PORTB ^= _BV(PB0); // toggle LED

  __LATCH_LOW;
    spi_transfer(0x02); // all outputs on
  __LATCH_HIGH;
  _delay_ms(500);

  PORTB ^= _BV(PB0); // toggle LED

  __LATCH_LOW;
    spi_transfer(0x04); // all outputs on
  __LATCH_HIGH;
  _delay_ms(500);

  PORTB ^= _BV(PB0); // toggle LED

  __LATCH_LOW;
    spi_transfer(0x07); // all outputs on
  __LATCH_HIGH;
  _delay_ms(500);

  PORTB ^= _BV(PB0); // toggle LED

  __LATCH_LOW;
    spi_transfer(0x00); // all outputs on
  __LATCH_HIGH;
  _delay_ms(500);

  PORTB ^= _BV(PB0); // toggle LED
}

int main(void) {
	setup();
	for(;;) {
		loop();
	}
};

/*
Functions dealing with hardware specific jobs / settings
*/

uint8_t spi_transfer(uint8_t data) {
  USIDR = data;
  USICR = (1<<USIOIE)|(1<<USIWM0)|(1<<USICS1)|(1<<USICLK);
  USISR = _BV(USIOIF); // clear flag

  while ( (USISR & _BV(USIOIF)) == 0 ) {
    USICR |= _BV(USITC);
  }
  return USIDR;
}
