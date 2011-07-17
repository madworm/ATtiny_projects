#define LATCH_LOW PORTB &= ~(1 << PB1)
#define LATCH_HIGH PORTB |= (1 << PB1)
#define DISPLAY_ON PORTB &= ~_BV(PB0)
#define DISPLAY_OFF PORTB |= _BV(PB0)
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

#define OCR1A_MAX 250U		// DON'T change this, must fit in an uint8_t
#define FADE_DELAY 32U

// at 9600 one bit should take 0.104ms
#define HALF_BIT_DELAY          48  // tuned a bit after looking at the sample times
#define FULL_BIT_DELAY          96 // with my newly acquired OLS from Dangerous Prototypes ;-)
#define THREE_HALFS_BIT_DELAY   150

#define ENABLE_PCINT0_VECT  GIMSK |= _BV(PCIE0);
#define DISABLE_PCINT0_VECT GIMSK &= ~_BV(PCIE0);
#define CLEAR_PCINT0_FLAG   GIFR |= _BV(PCIF0);

#define ENABLE_TIM0_COMPA_VECT  TIMSK0 |= _BV(OCIE0A)
#define DISABLE_TIM0_COMPA_VECT TIMSK0 &= ~_BV(OCIE0A)
#define CLEAR_TIM0_COMPA_FLAG   TIFR0 = _BV(OCF0A); // clear the flag by writing a 1 (see datasheet);

inline void setup_hw(void);
void loop(void);
void kitchen_lights(uint8_t channel);
void fade_in(uint8_t start_at, uint16_t fade_delay);
void fade_out(uint8_t start_at, uint16_t fade_delay);
void adc_test(uint8_t channel);
void soft_uart_rx_test(void);
void soft_uart_tx(uint8_t byte);
uint8_t read_adc(uint8_t channel);
void delay(uint16_t ticks);
uint16_t time(void);
inline void signal_reset(void);
inline void setup_system_ticker(void);
inline void setup_timer1_ctc(void);
inline void setup_soft_uart_rx_isr(void);
inline uint8_t spi_transfer(uint8_t data);
int main(void);
