#define __LATCH_LOW PORTB &= ~(1 << PB1)
#define __LATCH_HIGH PORTB |= (1 << PB1)
#define __DISPLAY_ON PORTB &= ~_BV(PB0)
#define __DISPLAY_OFF PORTB |= _BV(PB0)
#define __LED_ON PORTB |= _BV(PB2)
#define __LED_OFF PORTB &= ~_BV(PB2)

#define __step_delay 1500U	// used to let the amp-meter settle on a current value

#define __OCR1A_max 128U		// DON'T change this
#define __fade_delay 256U

inline void setup_hw(void);
void loop(void);
void kitchen_lights(void);
void fade_in(uint8_t start_at);
void fade_out(uint8_t start_at);
void delay(uint32_t ticks);
uint32_t time(void);
inline void signal_reset(void);
inline void setup_system_ticker(void);
inline void setup_timer1_ctc(void);
inline uint8_t spi_transfer(uint8_t data);
int main(void);
