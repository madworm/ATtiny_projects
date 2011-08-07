#ifndef spi_h
#define spi_H

#define LATCH_LOW PORTB &= ~(1 << PB4)
#define LATCH_HIGH PORTB |= (1 << PB4)
#define LATCH LATCH_HIGH; LATCH_LOW

void usi_setup(void);
uint8_t spi_transfer(uint8_t data);

#endif
