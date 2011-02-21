#define __LATCH_LOW PORTB &= ~(1 << PB4)
#define __LATCH_HIGH PORTB |= (1 << PB4)
#define __DISPLAY_ON PORTB &= ~_BV(PB2)
#define __DISPLAY_OFF PORTB |= _BV(PB2)
#define __LED0_ON PORTB |= _BV(PB0)
#define __LED0_OFF PORTB &= ~_BV(PB0)
#define __LED1_ON PORTB |= _BV(PB1)
#define __LED1_OFF PORTB &= ~_BV(PB1)

#define __step_delay 1500U	// used to let the amp-meter settle on a current value

#define __OCR1A_max 80U	// this adjusts the PWM frequency
#define __fade_delay 64U

typedef struct {
	uint8_t number;
	uint8_t dutycycle;
} led_t;

void setup_hw(void);
void populate_sorted(void);
void loop(void);
void no_isr_demo(void);
void fader(void);
void delay(uint32_t ticks);
void current_calib(void);
void bubbleSort(volatile led_t ** array, uint8_t size);
void setup_system_ticker(void);
uint32_t time(void);
void setup_timer1_ctc(void);
uint8_t spi_transfer(uint8_t data);
int main(void);
