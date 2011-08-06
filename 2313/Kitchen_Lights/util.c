#include <avr/io.h>
#include <stdint.h>
#include "util.h"
#include "system_ticker.h"

void signal_reset(void)
{
    S_LED_TOGGLE;
    delay(1000);
    S_LED_TOGGLE;
    delay(1000);
    S_LED_TOGGLE;
    delay(1000);
    S_LED_TOGGLE;
    delay(1000);
}
