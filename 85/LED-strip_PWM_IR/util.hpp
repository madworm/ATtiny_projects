#ifndef util_h
#define util_h

#define LED_PORT PORTB
#define LED_DIR  DDRB
#define LED_pin  PB2

#define LED_on      LED_DIR |= _BV(LED_pin); LED_PORT |= _BV(LED_pin)
#define LED_off     LED_PORT &= ~_BV(LED_pin);
#define LED_idle    LED_PORT |= _BV(LED_pin); LED_DIR &= ~_BV(LED_pin)

#endif
