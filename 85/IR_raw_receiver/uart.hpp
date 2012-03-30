#ifndef uart_h
#define uart_h

#define BAUDSPEED 9600 // valid: 9600, 38400 (must be tweaked if the receiver is not a real uart with accurate clock

#ifdef BAUDSPEED 38400
// 38400 values (reduced a bit, running on 16MHz xtal for testing)
// may not work with ceramic resonator
#define HALF_BIT_DELAY         11 // tuned a bit after looking at the sample times
#define FULL_BIT_DELAY         23 // with my newly acquired OLS from Dangerous Prototypes ;-)
#define THREE_HALFS_BIT_DELAY  35
#endif

#ifdef BAUDSPEED 9600
#define HALF_BIT_DELAY         52
#define FULL_BIT_DELAY        104
#define THREE_HALFS_BIT_DELAY 156
#endif

#ifdef BAUDSPEED 10417 // the ATtiny24 project want this (no real uart...)
#define HALF_BIT_DELAY         48
#define FULL_BIT_DELAY         96
#define THREE_HALFS_BIT_DELAY 144
#endif

#ifdef __AVR_ATmega168__
#define UART_DIR  DDRD
#define UART_PORT PORTD
#define UART_PIN PD1 // Arduino digital pin #1 (tx)
#endif

#ifdef __AVR_ATtiny85__
#define UART_DIR  DDRB
#define UART_PORT PORTB
#define UART_PIN PB2
#endif

void soft_uart_init(void);
void soft_uart_write(uint8_t byte);
void soft_uart_send(uint8_t number);
void soft_uart_send(int8_t number);
void soft_uart_send(uint16_t number);
void soft_uart_send(int16_t number);
void soft_uart_send(const char * string);

#endif
