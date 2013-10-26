#ifndef main_h
#define main_h

void setup_hw(void);
void fade(uint8_t from, uint8_t to, uint16_t f_delay, uint8_t color);
void breathe(uint16_t breathe_delay, uint8_t color, int8_t times);
void burst(uint8_t bursts, uint16_t burst_delay, uint8_t pulses, uint16_t pulse_delay, uint8_t color);
void rainbow(uint16_t rainbow_delay);
void helper(uint8_t color, uint8_t value);
uint8_t PB0_PB2_shorted(void);
int main(void);

#endif
