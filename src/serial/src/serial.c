#include "serial.h"
#include "serial_cfg.h"
#include "serial_prv.h"
#include "os_cfg.h"
#include "types.h"
#include "bits.h"

#include <avr/pgmspace.h>

void Serial_Init()
{
    Serial_HAL_Init_HW();

    #if SERIAL_ASYNC_RX == ON
    Serial_HAL_EnableRxInterrupts();
    #endif

    #if SERIAL_ASYNC_TX == ON
    Serial_HAL_EnableTxInterrupts();
    #endif
}

Std_ReturnType Serial_WriteByte(uint8_t chr)
{
    Serial_HAL_WriteByte(chr);
    while( !Serial_HAL_TxIsReady() );
    return Status_OK;
}

Std_ReturnType Serial_WriteBytes(void *buffer, int length)
{
    while ( length-- > 0 )
    {
        Serial_WriteByte(READ_PU8(buffer++));
    }

    return Status_OK;
}

Std_ReturnType Serial_NewLine()
{
    static byte nl[2] = {'\r', '\n'};
    return Serial_WriteBytes(nl, 2);
}

Std_ReturnType Serial_PrintString(const void* string, bool newLine, bool progMem)
{
    byte b = 0;

    while (TRUE)
    {
        b = progMem ? pgm_read_byte(string) : READ_PU8(string);
        if (b != 0) Serial_WriteByte(b);
        else break;
        string++;
    }

    if ( newLine )
    {
        Serial_NewLine();
    }

    Serial_WriteByte(0);

    return Status_OK;
}

#if SERIAL_ASYNC_RX == OFF
int Serial_Read(void *buffer, int buffer_len)
{
    int rcvd_len = 0;

    // Stop when buffer full
    while ( rcvd_len < buffer_len )
    {
        while( !Serial_HAL_RxIsReady() ); // Wait for RX complete
        UINT8_PTR(buffer)[rcvd_len++] = Serial_HAL_ReadByte();
    }

    return rcvd_len;
}
#endif // SERIAL_ASYNC_RX == OFF
