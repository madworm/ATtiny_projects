#ifndef system_ticker_h
#define system_ticker_h

#include "breathe_mode.h"

#ifdef BREATHE_MODE
extern volatile uint16_t breathe_delay;
#endif

void system_ticker_setup(void);
uint16_t time(void);
void delay(uint16_t ticks);

#endif
