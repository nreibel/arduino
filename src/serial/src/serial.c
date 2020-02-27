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

Std_ReturnType Serial_WriteByte(const uint8_t chr)
{
    Serial_HAL_WriteByte(chr);
    while( !Serial_HAL_TxIsReady() );
    return Status_OK;
}

Std_ReturnType Serial_Print( const void * buffer, unsigned int length )
{
    while ( length-- > 0 )
    {
        Serial_HAL_WriteByte( READ_PU8(buffer++) );
        while( !Serial_HAL_TxIsReady() ); // Wait for TX complete
    }

    return Status_OK;
}

Std_ReturnType Serial_PrintString(const char* string )
{
    while ( *string != 0 )
    {
        Serial_HAL_WriteByte( READ_PU8(string++) );
        while( !Serial_HAL_TxIsReady() ); // Wait for TX complete
    }

    return Status_OK;
}

#if SERIAL_ASYNC_RX == OFF
Std_ReturnType Serial_Read(void *buffer, const unsigned int buffer_len, unsigned int *rcvd_len)
{
    Std_ReturnType retval = Status_Not_OK;
    *rcvd_len = 0;

    // Stop when buffer full
    while ( *rcvd_len < buffer_len )
    {
        while( !Serial_HAL_RxIsReady() ); // Wait for RX complete
        uint8_t received = Serial_HAL_ReadByte();

        // Stop if last char is a line break
        if ( received == SERIAL_LINE_TERMINATOR)
        {
            break;
        }
        else
        {
            UINT8_PTR(buffer)[*(rcvd_len++)] = received;
            retval = Status_OK; // At least 1 byte has been read
        }
    }

    return retval;
}
#endif // SERIAL_ASYNC_RX == OFF
