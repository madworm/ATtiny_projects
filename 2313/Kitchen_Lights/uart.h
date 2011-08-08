#ifndef uart_h
#define uart_h

void uart_setup(void);
void uart_half_duplex_test();
void uart_send(uint8_t byte);
void uart_send_string(const char * string);
void uart_send_pgm_string(const char * string);
uint8_t uart_read(void);
#define uart_avail() uart_rx_buffer_fullness()
uint8_t uart_rx_buffer_fullness(void);
uint8_t uart_tx_buffer_fullness(void);

#endif
