#include "serial.h"
#include "serial_cfg.h"
#include "serial_prv.h"
#include "os_cfg.h"
#include "types.h"
#include "bits.h"

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

Std_ReturnType Serial_Print(void *string )
{
    do
    {
        Serial_WriteByte(READ_PU8(string));
    }
    while ( READ_PU8(string++) != 0 );

    return Status_OK;
}

Std_ReturnType Serial_Println(void *string )
{
    while ( READ_PU8(string) != 0 )
    {
        Serial_WriteByte(READ_PU8(string++));
    }
    Serial_WriteByte('\r');
    Serial_WriteByte('\n');
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
