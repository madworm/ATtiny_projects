#ifndef uart_h
#define uart_h

// valid: 9600, 38400 (must be tweaked if the receiver is not a real uart with accurate clock
#define BAUDSPEED 9600

// these values are not the nominal ones, but tweaked by looking a the actual signal
#if BAUDSPEED == 38400
#define HALF_BIT_DELAY 11
#endif

// these values are not the nominal ones, but tweaked by looking a the actual signal
#if BAUDSPEED == 9600
#define HALF_BIT_DELAY 50
#endif

// these values are not the nominal ones, but tweaked by looking a the actual signal
// the ATtiny24 project want this (no real uart...)
#if BAUDSPEED == 10417
#define HALF_BIT_DELAY 48
#endif

#define UART_DIR    DDRB
#define UART_PIN    PINB
#define UART_PORT   PORTB
#define UART_pin    PB3

void soft_uart_init(void);
void soft_uart_write(uint8_t byte);

void soft_uart_send(uint8_t number);
void soft_uart_send(uint8_t number, uint8_t base);
void soft_uart_send(int8_t number);
void soft_uart_send(uint16_t number);
void soft_uart_send(int16_t number);
void soft_uart_send(const char * string, uint8_t times);

#endif
