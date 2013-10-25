#ifndef main_h
#define main_h

void setup_hw(void);
void fade(uint8_t from, uint8_t to, uint16_t f_delay, uint8_t color);
void breathe(uint16_t b_delay, uint8_t color, int8_t times);
uint8_t PB0_PB2_shorted(void);
int main(void);

#endif
