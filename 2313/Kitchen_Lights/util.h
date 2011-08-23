#ifndef util_h
#define util_h

#define S_LED_ON PORTD |= _BV(PD5)
#define S_LED_OFF PORTD &= ~_BV(PD5)
#define S_LED_BLINK S_LED_ON; S_LED_OFF
#define S_LED_TOGGLE PORTD ^= _BV(PD5)

#define RX_LED_ON       PORTB |= _BV(PB2)
#define RX_LED_OFF      PORTB &= ~_BV(PB2)
#define RX_LED_BLINK    RX_LED_ON; RX_LED_OFF
#define RX_LED_TOGGLE   PORTB ^= _BV(PB2)

#define TX_LED_ON       PORTD |= _BV(PD6)
#define TX_LED_OFF      PORTD &= ~_BV(PD6)
#define TX_LED_BLINK    TX_LED_ON; TX_LED_OFF
#define TX_LED_TOGGLE   PORTD ^= _BV(PD6)

void signal_reset(void);

#endif
