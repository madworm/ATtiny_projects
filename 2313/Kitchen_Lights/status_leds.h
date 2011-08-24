#ifndef status_leds_h
#define status_leds_h

#define LEFT_LED_ON         DDRD |= _BV(PD2); PORTD &= ~_BV(PD2)
#define LEFT_LED_OFF        DDRD &= ~_BV(PD2); PORTD |= _BV(PD2)
#define LEFT_LED_FLASH      LEFT_LED_ON; LEFT_LED_OFF
#define LEFT_LED_BLINK      LEFT_LED_ON; delay(250); LEFT_LED_OFF; delay(250)

#define MID_LED_ON          DDRD |= _BV(PD3); PORTD &= ~_BV(PD3)
#define MID_LED_OFF         DDRD &= ~_BV(PD3); PORTD |= _BV(PD3)
#define MID_LED_FLASH       MID_LED_ON; MID_LED_OFF
#define MID_LED_BLINK       MID_LED_ON; delay(250); MID_LED_OFF; delay(250)

#define RIGHT_LED_ON        DDRD |= _BV(PD4); PORTD &= ~_BV(PD4)
#define RIGHT_LED_OFF       DDRD &= ~_BV(PD4); PORTD |= _BV(PD4)
#define RIGHT_LED_FLASH     RIGHT_LED_ON; RIGHT_LED_OFF
#define RIGHT_LED_BLINK     RIGHT_LED_ON; delay(250); RIGHT_LED_OFF; delay(250)

#define S_LED_ON            PORTD |= _BV(PD5)
#define S_LED_OFF           PORTD &= ~_BV(PD5)
#define S_LED_FLASH         S_LED_ON; S_LED_OFF
#define S_LED_BLINK         S_LED_ON; delay(250); S_LED_OFF; delay(250)
#define S_LED_TOGGLE        PORTD ^= _BV(PD5)

#define RX_LED_ON           PORTB |= _BV(PB2)
#define RX_LED_OFF          PORTB &= ~_BV(PB2)
#define RX_LED_FLASH        RX_LED_ON; RX_LED_OFF
#define RX_LED_BLINK        RX_LED_ON; delay(500); RX_LED_OFF; delay(250)
#define RX_LED_TOGGLE       PORTB ^= _BV(PB2)

#define TX_LED_ON           PORTD |= _BV(PD6)
#define TX_LED_OFF          PORTD &= ~_BV(PD6)
#define TX_LED_FLASH        TX_LED_ON; TX_LED_OFF
#define TX_LED_BLINK        TX_LED_ON; delay(250); TX_LED_OFF; delay(250)
#define TX_LED_TOGGLE       PORTD ^= _BV(PD6)

void status_leds_test(void);

#endif
