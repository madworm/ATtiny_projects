#ifndef button_h
#define button_H

#define LEFT_LED_ON     DDRD |= _BV(PD2); PORTD &= ~_BV(PD2)
#define LEFT_LED_OFF    DDRD &= ~_BV(PD2); PORTD |= _BV(PD2)
#define LEFT_LED_BLINK  LEFT_LED_OFF; LEFT_LED_ON; delay(50); LEFT_LED_OFF

#define MID_LED_ON      DDRD |= _BV(PD3); PORTD &= ~_BV(PD3)
#define MID_LED_OFF     DDRD &= ~_BV(PD3); PORTD |= _BV(PD3)
#define MID_LED_BLINK   MID_LED_OFF; MID_LED_ON; delay(50); MID_LED_OFF

#define RIGHT_LED_ON       DDRD |= _BV(PD4); PORTD &= ~_BV(PD4)
#define RIGHT_LED_OFF      DDRD &= ~_BV(PD4); PORTD |= _BV(PD4)
#define RIGHT_LED_BLINK    RIGHT_LED_OFF; RIGHT_LED_ON; delay(50); RIGHT_LED_OFF

typedef enum SWITCHES_STATE {
    SW_ALL_OPEN,
    SW_LEFT_PRESSED,
    SW_MIDDLE_PRESSED,
    SW_RIGHT_PRESSED,
    SW_LEFT_MIDDLE_PRESSED,
    SW_RIGHT_MIDDLE_PRESSED
} SWITCHES_STATE_t;

void button_setup();
void button_test();
SWITCHES_STATE_t button_read_state();

#endif
