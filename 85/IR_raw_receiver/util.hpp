#ifndef util_hpp
#define util_hpp

#if defined(__AVR_ATmega168__)
#define LED_DIR  DDRB
#define LED_PIN  PINB
#define LED_PORT PORTB
#define LED_pin  PB5
#endif

#if defined(__AVR_ATtiny85__)
#define LED_DIR  DDRB
#define LED_PIN  PINB
#define LED_PORT PORTB
#define LED_pin  PB0
#endif

// set as output + HIGH
#define LED_ON LED_DIR |= _BV(LED_pin); LED_PORT |= _BV(LED_pin)
// set LOW + as input (pull-up off)
#define LED_OFF LED_PORT &= ~ _BV(LED_pin); LED_DIR &= ~_BV(LED_pin)
#define TOGGLE_LED LED_DIR |= _BV(LED_pin); LED_PORT ^= _BV(LED_pin)

#endif
