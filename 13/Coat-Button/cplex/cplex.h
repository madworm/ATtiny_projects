#ifndef cplex_h
#define cplex_h

/*
 * +-------------------------------------------------------------+
 * | LED1 is top center (above floral shape), counting clockwise |
 * | CATHODES point to the center of the board                   |
 * +-------------------------------------------------------------+
 *
 * ALL OFF:
 * --------
 *
 * DDRB  = 0x00;
 * PORTB = 0x00;
 *
 * LED1 ON:
 * --------
 *
 * DDRB  = (_BV(PB3 | _BV(PB2));
 * PORTB = (_BV(PB2));
 *
 * LED2 ON:
 * --------
 *
 * DDRB  = (_BV(PB3) | _BV(PB2));
 * PORTB = (_BV(PB3));
 *
 * LED3 ON:
 * --------
 *
 * DDRB  = (_BV(PB2) | _BV(PB1));
 * PORTB = (_BV(PB1));
 *
 * LED4 ON:
 * --------
 *
 * DDRB  = (_BV(PB2) | _BV(PB1));
 * PORTB = (_BV(PB2));
 *
 * LED5 ON:
 * --------
 *
 * DDRB  = (_BV(PB1 | _BV(PB0));
 * PORTB = (_BV(PB0));
 *
 * LED6 ON:
 * --------
 *
 * DDRB  = (_BV(PB1) | _BV(PB0));
 * PORTB = (_BV(PB1));
 *
 * LED7 ON:
 * --------
 *
 * DDRB  = (_BV(PB3) | _BV(PB0));
 * PORTB = (_BV(PB0));
 *
 * LED8 ON:
 * --------
 *
 * DDRB  = (_BV(PB3) | _BV(PB0));
 * PORTB = (_BV(PB3));
 *
 */

#endif
