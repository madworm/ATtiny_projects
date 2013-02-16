#ifndef system_ticker_h
#define system_ticker_h

#define BUTTON_STATE			4
#define BUTTON_WAS_PRESSED		3
#define BUTTON_WAS_RELEASED		2
#define ENC_COUNT_UP			1	
#define ENC_COUNT_DOWN			0

void system_ticker_setup(void);
uint32_t millis(void);
uint32_t micros(void);
void delay(uint32_t ms);
void delayMicroseconds(uint16_t us);
uint8_t encoder_get(void);

#endif
