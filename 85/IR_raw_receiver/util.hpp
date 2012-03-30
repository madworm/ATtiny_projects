#ifndef util_hpp
#define util_hpp

#ifdef __AVR_ATmega168__
#define LED_DIR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PB5
#endif

#ifdef __AVR_ATtiny85__
#define LED_DIR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PB0
#endif

// set as output + HIGH
#define LED_ON LED_DIR |= _BV(LED_PIN); LED_PORT |= _BV(LED_PIN)
// set LOW + as input (pull-up off)
#define LED_OFF LED_PORT &= ~ _BV(LED_PIN); LED_DIR &= ~_BV(LED_PIN)
#define TOGGLE_LED LED_DIR |= _BV(LED_PIN); LED_PORT ^= _BV(LED_PIN)

#endif
