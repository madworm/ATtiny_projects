#ifndef button_h
#define button_H

typedef enum SWITCHES_STATE {
    SW_ALL_OPEN,
    SW_LEFT_PRESSED,
    SW_MIDDLE_PRESSED,
    SW_RIGHT_PRESSED,
    SW_LEFT_MIDDLE_PRESSED,
    SW_RIGHT_MIDDLE_PRESSED
} SWITCHES_STATE_t;

void button_setup();
void button_test();
SWITCHES_STATE_t button_read_state();

#endif
