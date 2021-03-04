#include "types.h"
#include "bits.h"
#include "serial_prv.h"
#include "serial_cfg.h"

#if SERIAL_ASYNC_TX == ON

volatile void *s_txBuffer = NULL_PTR;
volatile int s_txLength = 0;

void serial_hal_isr_tx()
{
    if (s_txLength > 0)
    {
        uint8_t b = READ_PU8(s_txBuffer++);
        serial_hal_write_byte(b);
        s_txLength--;
    }
    else
    {
        s_txBuffer = NULL_PTR;
        s_txLength = 0;
    }
}

void serial_tx_ready(bool *ready)
{
    *ready = FALSE;
    if (s_txBuffer == NULL_PTR && s_txLength == 0)
    {
        *ready = TRUE;
    }
}

void serial_set_tx_buffer(void *buffer, int length)
{
    s_txBuffer = buffer;
    s_txLength = length;

    // Kickstart transmission
    serial_hal_write_byte( READ_PU8(s_txBuffer++) );
    s_txLength--;
}

#endif // SERIAL_ASYNC_TX
