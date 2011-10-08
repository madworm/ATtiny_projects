#ifndef main_h
#define main_h

typedef enum LAMP_JOB {
    LJ_NOP,
    LJ_MANUAL_UP,
    LJ_MANUAL_DOWN,
    LJ_FADE_IN,
    LJ_FADE_OUT,
    LJ_SEND_REMOTE_UP,
    LJ_SEND_REMOTE_DOWN,
    LJ_SEND_REMOTE_FADE_IN,
    LJ_SEND_REMOTE_FADE_OUT,
    LJ_RECVD_REMOTE_UP,
    LJ_RECVD_REMOTE_DOWN,
    LJ_RECVD_REMOTE_FADE_IN,
    LJ_RECVD_REMOTE_FADE_OUT
} LAMP_JOB_t;

typedef enum LAMP_STATE {
    LS_OFF,
    LS_FULLY_ON,
    LS_ON
} LAMP_STATE_t;

void eval_switch_state(SWITCHES_STATE_t state,LAMP_JOB_t first_job,LAMP_JOB_t second_job);
void process_lamp_job(LAMP_JOB_t job);

void setup_hw(void);
void kitchen_lights(void);

int main(void);

#endif
