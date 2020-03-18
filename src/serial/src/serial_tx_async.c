#include "types.h"
#include "bits.h"
#include "serial_prv.h"
#include "serial_cfg.h"

#if SERIAL_ASYNC_TX == ON

volatile void *s_txBuffer = NULL_PTR;
volatile int s_txLength = 0;

void Serial_HAL_ISR_Tx()
{
    if (s_txLength > 0)
    {
        Serial_HAL_WriteByte( READ_PU8(s_txBuffer++) );
        s_txLength--;
    }
    else
    {
        s_txBuffer = NULL_PTR;
        s_txLength = 0;
    }
}

Std_ReturnType Serial_TxReady(bool *ready)
{
    *ready = FALSE;
    if (s_txBuffer == NULL_PTR && s_txLength == 0)
    {
        *ready = TRUE;
    }
    return Status_OK;
}

Std_ReturnType Serial_SetTxBuffer(void *buffer, int length)
{
    s_txBuffer = buffer;
    s_txLength = length;

    // Kickstart transmission
    Serial_HAL_WriteByte( READ_PU8(s_txBuffer++) );
    s_txLength--;

    return Status_OK;
}

#endif // SERIAL_ASYNC_TX
