#ifndef system_ticker_h
#define system_ticker_h

extern volatile uint8_t brightness_a;

void system_ticker_setup(void);
uint16_t time(void);
void delay(uint16_t ticks);
void __delay_ms(uint16_t ms);

#endif
