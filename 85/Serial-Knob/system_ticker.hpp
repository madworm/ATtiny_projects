#ifndef system_ticker_h
#define system_ticker_h

#define BUTTON_STATE			2
#define BUTTON_WAS_PRESSED		1
#define BUTTON_WAS_RELEASED		0
#define ENC_COUNTS              7
#define ENC_VELOCITY			8

void system_ticker_setup(void);
uint32_t millis(void);
uint32_t micros(void);
void delay(uint32_t ms);
void delayMicroseconds(uint16_t us);
int16_t encoder_get(uint8_t whatbit);

#endif
