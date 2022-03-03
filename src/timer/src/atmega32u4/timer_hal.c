#include "types.h"
#include "timer.h"

const uint8_t __timer_prescaler_table[NUMBER_OF_TIMERS][NUMBER_OF_PRESCALERS] = {
    [TIMER0] = {
        [TIMER_PRESCALER_STOPPED] = TIMER0_LL_CS_STOPPED,
        [TIMER_PRESCALER_1]       = TIMER0_LL_CS_CLKIO,
        [TIMER_PRESCALER_8]       = TIMER0_LL_CS_DIV8,
        [TIMER_PRESCALER_32]      = 0xFF,
        [TIMER_PRESCALER_64]      = TIMER0_LL_CS_DIV64,
        [TIMER_PRESCALER_128]     = 0xFF,
        [TIMER_PRESCALER_256]     = TIMER0_LL_CS_DIV256,
        [TIMER_PRESCALER_1024]    = TIMER0_LL_CS_DIV1024,
    }
};
