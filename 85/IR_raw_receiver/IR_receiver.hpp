#ifndef IR_receiver_h
#define IR_receiver_h

// all elements EXCEPT (REPEAT_CODE, MISMATCH and NOT_SURE_YET) must be in the same
// order (if present at all) as in the PROGMEM array IRsignals! These 3 special elements
// must ALWAYS be there.
typedef enum {
    VOL_DOWN,
    VOL_UP,
    ARROW_UP,
    ARROW_DOWN,
    // DO NOT EDIT THESE LAST 3!
    REPEAT_CODE = 66,
    MISMATCH = 77,		// only returned if something goes terribly wrong
    NOT_SURE_YET = 99,	// this should NEVER be returned as a result
} IR_code_t;

void init_IR(void);
uint8_t IR_available(void);
IR_code_t eval_IR_code(void);

#endif
