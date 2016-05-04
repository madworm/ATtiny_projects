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
 * DDRB  = ( _BV(PB3) | _BV(PB2) );
 * PORTB = ( _BV(PB2) );
 *
 * LED2 ON:
 * --------
 *
 * DDRB  = ( _BV(PB3) | _BV(PB2) );
 * PORTB = ( _BV(PB3) );
 *
 * LED3 ON:
 * --------
 *
 * DDRB  = ( _BV(PB2) | _BV(PB1) );
 * PORTB = ( _BV(PB1) );
 *
 * LED4 ON:
 * --------
 *
 * DDRB  = ( _BV(PB2) | _BV(PB1) );
 * PORTB = ( _BV(PB2) );
 *
 * LED5 ON:
 * --------
 *
 * DDRB  = ( _BV(PB1) | _BV(PB0) );
 * PORTB = ( _BV(PB0) );
 *
 * LED6 ON:
 * --------
 *
 * DDRB  = ( _BV(PB1) | _BV(PB0) );
 * PORTB = ( _BV(PB1) );
 *
 * LED7 ON:
 * --------
 *
 * DDRB  = ( _BV(PB3) | _BV(PB0) );
 * PORTB = ( _BV(PB0) );
 *
 * LED8 ON:
 * --------
 *
 * DDRB  = ( _BV(PB3) | _BV(PB0) );
 * PORTB = ( _BV(PB3) );
 *
 */

#define ALL_OFF  { DDRB = 0x00; PORTB = 0x00; }
#define ONE_ON   { DDRB  = ( _BV(PB3) | _BV(PB2) ); PORTB = ( _BV(PB2) ); }
#define TWO_ON   { DDRB  = ( _BV(PB3) | _BV(PB2) ); PORTB = ( _BV(PB3) ); }
#define THREE_ON { DDRB  = ( _BV(PB2) | _BV(PB1) ); PORTB = ( _BV(PB1) ); }
#define FOUR_ON  { DDRB  = ( _BV(PB2) | _BV(PB1) ); PORTB = ( _BV(PB2) ); }
#define FIVE_ON  { DDRB  = ( _BV(PB1) | _BV(PB0) ); PORTB = ( _BV(PB0) ); }
#define SIX_ON   { DDRB  = ( _BV(PB1) | _BV(PB0) ); PORTB = ( _BV(PB1) ); }
#define SEVEN_ON { DDRB  = ( _BV(PB3) | _BV(PB0) ); PORTB = ( _BV(PB0) ); }
#define EIGHT_ON { DDRB  = ( _BV(PB3) | _BV(PB0) ); PORTB = ( _BV(PB3) ); }

/*
 * values needed to turn an individual LED to ON
 *
 * 3-state charlie plexing 
 *
 */
uint8_t DDRB_array[8] =  { 8+4, 8+4, 4+2, 4+2, 2+1, 2+1, 8+1, 8+1 };
uint8_t PORTB_array[8] = { 4, 8, 2, 4, 1, 2, 1, 8 };

#endif
