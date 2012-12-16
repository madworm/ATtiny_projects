#ifndef main_h
#define main_h

#include "breathe_mode.h"

void setup_hw(void);
void initial_fade_in(void);

int main(void);

#ifdef BREATHE_MODE
void breathe(void);
#endif

#endif
