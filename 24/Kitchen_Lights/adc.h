#ifndef adc_h
#define adc_H

typedef enum SWITCHES_STATE {
    SW_ALL_OPEN,
    SW_LEFT_PRESSED,
    SW_MIDDLE_PRESSED,
    SW_RIGHT_PRESSED,
    SW_LEFT_MIDDLE_PRESSED,
    SW_RIGHT_MIDDLE_PRESSED
} SWITCHES_STATE_t;

void adc_test(uint8_t channel);
uint8_t adc_read(uint8_t channel);
SWITCHES_STATE_t adc_read_state(uint8_t channel);

/*
     board 1: tested with adc_test(1) and timer1 OFF !

     switches pressed    voltage     binary MSB...LSB     ADCH    state

     none                -.--        11111111            255     SW_ALL_OPEN
     1                   -.--        10000000            128     SW_LEFT_PRESSED
     2                   -.--        10101010            170     SW_MIDDLE_PRESSED
     3                   -.--        11001101            205     SW_RIGHT_PRESSED
     1+2                 -.--        01100110            102     SW_LEFT_MIDDLE_PRESSED
     3+2                 -.--        01001001            146     SW_RIGHT_MIDDLE_PRESSED


     board 2: tested with adc_test(1) and timer1 OFF !

     switches pressed    voltage     ADCH    state

     none                -.--        11111111            255     SW_ALL_OPEN
     1                   -.--        01111111            127     SW_LEFT_PRESSED
     2                   -.--        10101010            170     SW_MIDDLE_PRESSED
     3                   -.--        11001101            205     SW_RIGHT_PRESSED
     1+2                 -.--        01100110            102     SW_LEFT_MIDDLE_PRESSED
     3+2                 -.--        10010010            146     SW_RIGHT_MIDDLE_PRESSED


     board 3: tested with adc_test(1) and timer1 OFF !

     switches pressed    voltage     ADCH    state

     none                -.--        11111111            255     SW_ALL_OPEN
     1                   -.--        10000000            127     SW_LEFT_PRESSED
     2                   -.--        10101011            171     SW_MIDDLE_PRESSED
     3                   -.--        11001101            205     SW_RIGHT_PRESSED
     1+2                 -.--        01100110            102     SW_LEFT_MIDDLE_PRESSED
     3+2                 -.--        10010010            146     SW_RIGHT_MIDDLE_PRESSED
*/

#endif
