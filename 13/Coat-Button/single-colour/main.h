#ifndef main_h
#define main_h

void setup_hw(void);
void fade(uint8_t from, uint8_t to, uint16_t f_delay);
void breathe(uint16_t breathe_delay, int8_t times);
void burst(uint8_t bursts, uint16_t burst_delay, uint8_t pulses, uint16_t pulse_delay);
void helper(uint8_t value);
uint8_t PB0_PB2_shorted(void);
uint32_t pseudo_rand(void);
void flicker(void);
int main(void);

#endif
