#ifndef uart_h
#define uart_h

// at 9600 one bit should take 0.104ms
#define HALF_BIT_DELAY          48  // tuned a bit after looking at the sample times
#define FULL_BIT_DELAY          96 // with my newly acquired OLS from Dangerous Prototypes ;-)
#define THREE_HALFS_BIT_DELAY   136

#define ENABLE_PCINT0_VECT  GIMSK |= _BV(PCIE0)
#define DISABLE_PCINT0_VECT GIMSK &= ~_BV(PCIE0)
#define CLEAR_PCINT0_FLAG   GIFR |= _BV(PCIF0)

#define ENABLE_TIM0_COMPA_VECT  TIMSK0 |= _BV(OCIE0A)
#define DISABLE_TIM0_COMPA_VECT TIMSK0 &= ~_BV(OCIE0A)
#define CLEAR_TIM0_COMPA_FLAG   TIFR0 = _BV(OCF0A) // clear the flag by writing a 1 (see datasheet);

#define ENABLE_TIM1_COMPA_VECT  TIMSK1 |= _BV(OCIE1A)
#define DISABLE_TIM1_COMPA_VECT TIMSK1 &= ~_BV(OCIE1A)

// only for RX_FLAG debugging, as on same pin as the status led
// #define TOGGLE_LED
// #define LED_ON
// #define LED_OFF

void soft_uart_setup(void);
void soft_uart_send(uint8_t byte);
uint8_t soft_uart_read(void);
uint8_t soft_uart_peek(void);

void soft_uart_rx_test(void);

#endif
