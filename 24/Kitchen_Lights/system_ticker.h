#ifndef system_ticker_h
#define system_ticker_h

extern volatile uint16_t system_ticks; // this is a global variable

void setup_system_ticker(void);
uint16_t time(void);
void delay(uint16_t ticks);

#endif
