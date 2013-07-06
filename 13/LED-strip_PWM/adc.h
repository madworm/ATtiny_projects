#ifndef adc_h
#define adc_H

// ADC channels: #0 --> PB5, #1 --> PB2, #2 --> PB4 (POT2), #3 --> PB3 (POT1)
#define POT1 3
#define POT2 2

uint8_t adc_read(uint8_t channel);
void adc_init(void);

#endif
