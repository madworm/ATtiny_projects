#define __LATCH_LOW PORTB &= ~(1 << PB4)
#define __LATCH_HIGH PORTB |= (1 << PB4)
#define __DISPLAY_ON PORTB &= ~_BV(PB2)
#define __DISPLAY_OFF PORTB |= _BV(PB2)
#define __LED0_ON PORTB |= _BV(PB0)
#define __LED0_OFF PORTB &= ~_BV(PB0)
#define __LED1_ON PORTB |= _BV(PB1)
#define __LED1_OFF PORTB &= ~_BV(PB1)

#define __step_delay 1500U	// used to let the amp-meter settle on a current value

#define __OCR1A_max 64U		// DON'T change this
#define __fade_delay 1024U

typedef struct {
	uint8_t number;
	uint8_t dutycycle;
} led_t;

static inline void setup_hw(void);
static inline void populate_sorted(void);
static inline void loop(void);
static void no_isr_demo(void);
static inline void fader(void);
static void clear_buffer();
static void flip_buffers();
static void delay(uint32_t ticks);
static inline void current_calib(void);
static void bubbleSort(led_t ** array, uint8_t size);
static inline void setup_system_ticker(void);
uint32_t time(void);
static inline void setup_timer1_ctc(void);
static inline uint8_t spi_transfer(uint8_t data);
int main(void);
