#ifndef led_driver_h
#define led_driver_h

#define LATCH_LOW PORTB &= ~(1 << PB1)
#define LATCH_HIGH PORTB |= (1 << PB1)
#define DISPLAY_ON PORTB &= ~_BV(PB0)
#define DISPLAY_OFF PORTB |= _BV(PB0)

#define OCR1A_MAX 250U		// DON'T change this, must fit in an uint8_t
#define AUTO_FADE_IN_DELAY      64U
#define AUTO_FADE_OUT_DELAY     128U
#define MANUAL_FADE_IN_DELAY    128U // slow ( better for the eyes in the morning ;-) )
#define MANUAL_FADE_OUT_DELAY   128U

extern uint8_t brightness; // global variable

void led_driver_setup(void);
void fade_in(uint8_t start_at, uint16_t fade_delay);
void fade_out(uint8_t start_at, uint16_t fade_delay);

#endif
