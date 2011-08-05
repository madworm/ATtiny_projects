#ifndef util_h
#define util_h

#define LED_ON PORTB |= _BV(PB2)
#define LED_OFF PORTB &= ~_BV(PB2)
#define TOGGLE_LED PORTB ^= _BV(PB2)

// only for debugging
#define PA2_ON PORTA |= _BV(PA2)
#define PA2_OFF PORTA &= ~_BV(PA2)
#define PA3_ON PORTA |= _BV(PA3)
#define PA3_OFF PORTA &= ~_BV(PA3)
#define PA7_ON PORTA |= _BV(PA7)
#define PA7_OFF PORTA &= ~_BV(PA7)
#define RX_FLAG_ON  PORTB |= _BV(PB2)
#define RX_FLAG_OFF PORTB &= ~_BV(PB2)

void signal_reset(void);

#endif
