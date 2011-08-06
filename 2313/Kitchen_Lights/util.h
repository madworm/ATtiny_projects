#ifndef util_h
#define util_h

#define S_LED_ON PORTD |= _BV(PD5)
#define S_LED_OFF PORTD &= ~_BV(PD5)
#define S_LED_BLINK S_LED_ON; S_LED_OFF
#define S_LED_TOGGLE PORTD ^= _BV(PD5)

void signal_reset(void);

#endif
