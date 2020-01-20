#include "types.h"
#include "bits.h"
#include "serial_prv.h"
#include "serial_cfg.h"

#if SERIAL_ASYNC_RX == ON

static volatile boolean s_dataReady = FALSE;
static volatile uint8_t s_rxBuffer[SERIAL_RECEIVE_BUFFER_COUNT][SERIAL_RECEIVE_BUFFER_LENGTH];
static volatile unsigned int s_rxLength = 0;
static volatile uint8_t s_rxBufferId = 0;

void Serial_HAL_ISR_Rx()
{
    uint8_t received = Serial_HAL_ReadByte();
    if (received == SERIAL_LINE_TERMINATOR)
    {
        s_dataReady = TRUE;
    }
    else if (s_dataReady != TRUE && s_rxLength < SERIAL_RECEIVE_BUFFER_LENGTH)
    {
        s_rxBuffer[s_rxBufferId][s_rxLength++] = received;
    }
}

Std_ReturnType Serial_RxReady(boolean *ready)
{
    *ready = FALSE;
    if (s_dataReady || s_rxLength >= SERIAL_RECEIVE_BUFFER_LENGTH)
    {
        *ready = TRUE;
    }
    return Status_OK;
}

Std_ReturnType Serial_GetRxBuffer(void **buffer, unsigned int *rcvd_len)
{
    *buffer = UINT8_PTR(s_rxBuffer[s_rxBufferId]);
    *rcvd_len = s_rxLength;

    // Switch buffer
    INCREMENT_MOD(s_rxBufferId, SERIAL_RECEIVE_BUFFER_COUNT);
    s_rxLength = 0;
    s_dataReady = FALSE;

    return Status_OK;
}

#endif // SERIAL_ASYNC_RX
