#ifndef system_ticker_h
#define system_ticker_h

void system_ticker_init(void);
uint32_t millis(void);
uint32_t micros(void);
void delay(uint32_t ms);
void delayMicroseconds(uint16_t us);

#endif
