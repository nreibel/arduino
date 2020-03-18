#include "types.h"
#include "bits.h"
#include "serial_prv.h"
#include "serial_cfg.h"

#if SERIAL_ASYNC_RX == ON

// FIFO buffer
static void push(byte b);
static byte poll();
static byte s_rxBuffer[SERIAL_RECEIVE_BUFFER_LENGTH];
static byte *s_readPtr = s_rxBuffer;
static byte *s_writePtr = s_rxBuffer;

// Keep track of FIFO length
static int s_size = 0;

// Keep track of the number of strings in the buffer
static int s_nbstrings = 0;


static void push(byte b)
{
    *s_writePtr = b;

    s_writePtr++;
    s_size++;

    if (b == 0) s_nbstrings++;

    // At the end of buffer
    if (s_writePtr >= s_rxBuffer + SERIAL_RECEIVE_BUFFER_LENGTH)
    {
        s_writePtr = s_rxBuffer;
    }
}

static byte poll()
{
    byte b = *s_readPtr;

    s_readPtr++;
    s_size--;

    if (b == 0) s_nbstrings--;

    // At the end of buffer
    if (s_readPtr >= s_rxBuffer + SERIAL_RECEIVE_BUFFER_LENGTH)
    {
        s_readPtr = s_rxBuffer;
    }

    return b;
}

void Serial_HAL_ISR_Rx()
{
    push( Serial_HAL_ReadByte() );
}

bool Serial_HasByte()
{
    return s_size > 0;
}

bool Serial_HasString()
{
    return s_size > 0 && s_nbstrings > 0;
}

void Serial_ClearBuffer()
{
    s_readPtr = s_rxBuffer;
    s_writePtr = s_rxBuffer;
    s_size = 0;
    s_nbstrings = 0;
}

int Serial_Received()
{
    return s_size;
}

int Serial_ReadString(void *buffer, int len)
{
    int rcvd = 0;
    while(rcvd < len && s_size > 0 && s_nbstrings > 0)
    {
        byte b = poll();
        UINT8_PTR(buffer)[rcvd++] = b;
        if (b == 0) break;
    }
    return rcvd;
}

int Serial_Read(void *buffer, int len)
{
    int rcvd = 0;
    while(rcvd < len && s_size > 0)
    {
        UINT8_PTR(buffer)[rcvd++] = poll();
    }
    return rcvd;
}

#endif // SERIAL_ASYNC_RX
