#ifndef util_h
#define util_h

void delay_ms(uint16_t ms);
void delay_us(uint16_t us);
void pulse_PB0_ms(uint16_t ms); // LED-pin (via MOSFET)
void pulse_PB1_us(uint16_t us); // ISP-header pin #1

#endif
