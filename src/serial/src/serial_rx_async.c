#include "types.h"
#include "bits.h"
#include "serial.h"
#include "serial_prv.h"
#include "serial_cfg.h"

#if SERIAL_ASYNC_RX == ON

// FIFO buffer
static char buffer[SERIAL_RECEIVE_BUFFER_LENGTH];
static char *writePtr = buffer;
static int size = 0;

void Serial_HAL_ISR_Rx()
{
    char b = Serial_HAL_ReadByte();

    if (b == SERIAL_LINE_TERMINATOR)
    {
        // Terminate string
        *writePtr = 0;
        Serial_RxCallback(buffer, size+1);
        
        // Reset buffer
        writePtr = buffer;
        size = 0;
    }
    else
    {
        // Advance pointer
        // TODO : handle buffer full
        *writePtr = b;
        writePtr++;
        size++;
    }
}

#endif // SERIAL_ASYNC_RX

