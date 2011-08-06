#ifndef uart_h
#define uart_h

void uart_setup(void);
void uart_send(uint8_t byte);
uint8_t uart_read(void);
uint8_t uart_avail(void);
void uart_rx_test(void);

#endif
