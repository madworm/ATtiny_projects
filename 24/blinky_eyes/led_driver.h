#ifndef led_driver_h
#define led_driver_h

#define LATCH_LOW PORTB &= ~(1 << PB1)
#define LATCH_HIGH PORTB |= (1 << PB1)
#define LATCH LATCH_HIGH; LATCH_LOW
#define DISPLAY_ON PORTB &= ~_BV(PB0)
#define DISPLAY_OFF PORTB |= _BV(PB0)

#define __TRUE_RGB_OCR1A 255
#define __max_brightness 254
#define __fade_delay 64

void led_driver_setup(void);

void fader(void);
void fader_hue(void);
void set_led_red(uint8_t red);
void set_led_green(uint8_t green);
void set_led_blue(uint8_t blue);
void set_led_rgb(uint8_t red, uint8_t green, uint8_t blue);
void set_led_hsv(uint16_t hue, uint8_t sat, uint8_t val);

#endif
