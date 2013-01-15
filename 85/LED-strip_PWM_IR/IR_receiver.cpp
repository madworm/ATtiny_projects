#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h> // needed for abs()
#include <avr/pgmspace.h>
#include <stdint.h>
#include "system_ticker.hpp"
#include "uart.hpp"
#include "IR_receiver.hpp"

extern "C" {
#include "IR_codes.h"
}

void flip_buffers(void);
void zero_pulses(volatile uint16_t * array);

// double buffering
volatile uint16_t pulses_a[NUMPULSES];
volatile uint16_t pulses_b[NUMPULSES];
volatile uint16_t *pulses_write_to = pulses_a;
volatile uint16_t *pulses_read_from = pulses_b;

volatile uint32_t last_IR_activity = 0;

void init_IR(void)
{
    IR_DIR &= ~_BV(IR_pin); // set as input
    IR_PORT |= _BV(IR_pin); // pull-up on
    PC_ISR_REG |= _BV(PC_ISR_BIT); // turn pin-change interrupt on
    PC_PIN_MASK |= _BV(PC_PIN_MASK_BIT); // enable trigger source pin PCINT1 (PB1)
    zero_pulses(pulses_read_from);
    zero_pulses(pulses_write_to);
}

void __attribute__ ((noinline)) zero_pulses(volatile uint16_t * array)
{
    /*
     * If this functions appears on the garbage collect list of the linker
     * it was inlined! This can be forbidden with
     *
     * void __attribute__ ((noinline)) zero_pulses(...)
     *
     * It will then not show up on the removal list anymore.
     *
     */

    uint8_t ctr;
    for (ctr = 0; ctr < NUMPULSES; ctr++) {
        array[ctr] = 0;
    }
}

void flip_buffers(void)
{
    uint8_t _sreg = SREG;
    cli();
    volatile uint16_t *tmp;
    tmp = pulses_read_from;
    pulses_read_from = pulses_write_to;
    pulses_write_to = tmp;
    SREG = _sreg;
}

uint8_t IR_available(void)
{
    uint8_t _sreg = SREG;
    cli();
    if (last_IR_activity != 0
        && ((micros() - last_IR_activity) > MAXPULSE)) {
        flip_buffers();
        last_IR_activity = 0;
        SREG = _sreg;
        return 1;
    }
    SREG = _sreg;
    return 0;
}

IR_code_t eval_IR_code(void)
{
    uint8_t ctr1;
    uint8_t ctr2;
#if defined(TRANSLATE_REPEAT_CODE)
    static IR_code_t prev_IR_code = NOT_SURE_YET;
#endif
    IR_code_t IR_code;
    for (ctr2 = 0; ctr2 < NUMBER_OF_IR_CODES; ctr2++) {
#if defined(DEBUG)
        soft_uart_send(PSTR("\r\nChecking against array element #: "));
        soft_uart_send(ctr2);
        soft_uart_send(PSTR("\r\n"));
#endif
        IR_code = NOT_SURE_YET;

        for (ctr1 = 0; ctr1 < NUMPULSES - 6; ctr1++) {
            int16_t measured = (int16_t) (pulses_read_from[ctr1]);
            int16_t reference = (int16_t) pgm_read_word(&IRsignals[ctr2][ctr1]);
            uint16_t delta = (uint16_t) abs(measured - reference);
            uint16_t delta_repeat = (uint16_t) abs(measured - REPEAT_CODE_PAUSE);
#if defined(DEBUG)
            soft_uart_send(PSTR("measured: "));
            soft_uart_send(measured);
            soft_uart_send(PSTR(" - reference: "));
            soft_uart_send(reference);
            soft_uart_send(PSTR(" - delta: "));
            soft_uart_send(delta);
            soft_uart_send(PSTR(" - delta_rpt_code: "));
            soft_uart_send(delta_repeat);
#endif
            if (delta > (reference * FUZZINESS / 100)) {
                if (delta_repeat <
                    REPEAT_CODE_PAUSE * FUZZINESS / 100) {
#if defined(DEBUG)
                    soft_uart_send(PSTR(" - repeat code (ok)"));
#endif
                    IR_code = REPEAT_CODE;
                    break;
                }
#if defined(DEBUG)
                soft_uart_send(PSTR(" - (x)\r\n"));
#endif
                IR_code = MISMATCH;
                break;
            } else {
#if defined(DEBUG)
                soft_uart_send(PSTR(" - (ok)\r\n"));
#endif
            }
        }
        if (IR_code == REPEAT_CODE) {
#if defined(TRANSLATE_REPEAT_CODE)
            IR_code = prev_IR_code;
#endif
            break;
        }
        if (IR_code == NOT_SURE_YET) {
            IR_code = (IR_code_t) (ctr2);
            break;
        }
    }
#if defined(TRANSLATE_REPEAT_CODE)
    prev_IR_code = IR_code;
#endif
    zero_pulses(pulses_read_from);
    return IR_code;
}

void IR_scan(void)
{
    uint8_t ctr1;

    if(IR_available()) {

        soft_uart_send(PSTR("\n\r --- START --- \n\r"));
        soft_uart_send(PSTR("#define ir_code_1 \{\\ \n\r"));

        for (ctr1 = 0; ctr1 <= (NUMPULSES-2); ctr1++) {
            soft_uart_send(pulses_read_from[ctr]);
            soft_uart_send(PSTR(",\\ \n\r"));
        }

        soft_uart_send(pulses_read_from[NUMPULSES-1]);
        soft_uart_send(PSTR("\}\\ \n\r"));

        soft_uart_send(PSTR("\n\r --- END --- \n\r"));
        zero_pulses(pulses_read_from);
    }
}

ISR(PCINT0_vect)
{
    static uint8_t pulse_counter = 0;
    static uint32_t last_run = 0;
    uint32_t now = micros();
    uint32_t pulse_length = labs(now - last_run); // stdlib's abs() is only for ints, not for bigger stuff
    if (pulse_length > MAXPULSE) {
        zero_pulses(pulses_write_to);	// clear the buffer after a timeout has occurred
        pulse_counter = 0;
    }
    if (pulse_length < MINPULSE) {
        return;		// got some bouncing ? ignore that.
    }
    if (pulse_counter > 0) {
        pulses_write_to[pulse_counter - 1] =
            (uint16_t) (pulse_length / 10);
    } else {
        // exit asap
    }
    last_run = micros();
    pulse_counter++;
    if (pulse_counter > NUMPULSES) {
        pulse_counter = 0;
    }
    last_IR_activity = last_run;
}
