#include <avr/io.h>
#include <stdint.h>
#include "util.h"
#include "system_ticker.h"

void signal_reset(void)
{
    TOGGLE_LED;
    delay(1000);
    TOGGLE_LED;
    delay(1000);
    TOGGLE_LED;
    delay(1000);
    TOGGLE_LED;
    delay(1000);
}
