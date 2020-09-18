#include "serial.h"
#include "serial_tp.h"
#include "serial_tp_prv.h"

#define BUFFER_SIZE 32

typedef enum
{
    WaitData = 0,
    Receiving,
    Complete
} Serial_TP_State;

static Serial_TP_State state = WaitData;
static uint8_t data[BUFFER_SIZE] = {0};
static int length = 0;
static int expected = 0;

void Serial_TP_Init()
{
}

bool Serial_TP_Ready()
{
    return state == Complete;
}

int Serial_TP_Read(void* buffer, int len)
{
    int rcvd = 0;
    while(rcvd < len && rcvd < length && rcvd < expected)
    {
        UINT8_PTR(buffer)[rcvd] = data[rcvd];
        rcvd++;
    }

    // Reset state
    state = WaitData;
    length = 0;
    expected = 0;

    return rcvd;
}

void Serial_TP_Cyclic()
{
    switch(state)
    {
        case WaitData:
        {
            if ( Serial_Read(&expected, 1) > 0 ) state = Receiving;
            break;
        }
        case Receiving:
        {
            int max_read_length = BUFFER_SIZE - length;
            void *read_addr = data + length;
            int read_length = Serial_Read(read_addr, max_read_length);
            length += read_length;

            if (length > expected)
            {
                // TODO Too many bytes received
                state = Complete;
            }
            else if ( length == expected )
            {
                state = Complete;
            }
            else
            {
                // Carry on...
            }

            break;
        }
        case Complete:
        {
            // Waiting to read data...
            break;
        }
    }
}
