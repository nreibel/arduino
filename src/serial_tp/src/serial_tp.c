#include "serial_tp.h"
#include "serial_tp_cfg.h"
#include "types.h"
#include "bits.h"
#include "string.h"

static void Serial_SendResponse(Serial_TP_Response *rsp);

typedef enum {
    SERIAL_TP_RCVD_HEADER,
    SERIAL_TP_RCVD_FUNCTION,
    SERIAL_TP_RCVD_ADDRESS,
    SERIAL_TP_RCVD_LENGTH,
    SERIAL_TP_RCVD_DATA,
    SERIAL_TP_RCVD_TERMINATOR,
} serial_tp_state_t;

static void Serial_SendResponse(Serial_TP_Response *rsp)
{
    Serial_WriteBytes(rsp, 2 + rsp->length);
    Serial_WriteByte(SERIAL_TP_FRAME_TERMINATOR);
}

void Serial_RxCallback(uint8_t received)
{
    static Serial_TP_Request req = {0};
    static Serial_TP_Response rsp = {0};

    static uint8_t *rx_ptr = NULL_PTR;
    static int rx_len = 0;

    static serial_tp_state_t state = SERIAL_TP_RCVD_HEADER;

    switch(state)
    {
        case SERIAL_TP_RCVD_HEADER:
        {
            if (received == SERIAL_TP_REQUEST_HEADER)
            {
                memset(&rsp, 0, sizeof(Serial_TP_Response));
                state = SERIAL_TP_RCVD_FUNCTION;
            }
            else
            {
                rsp.status = SERIAL_TP_RETCODE_INVALID_HEADER;
                Serial_SendResponse(&rsp);
            }
            break;
        }

        case SERIAL_TP_RCVD_FUNCTION:
        {
            req.function = received;
            state = SERIAL_TP_RCVD_ADDRESS;
            break;
        }

        case SERIAL_TP_RCVD_ADDRESS:
        {
            req.address = received;
            state = SERIAL_TP_RCVD_LENGTH;
            break;
        }

        case SERIAL_TP_RCVD_LENGTH:
        {
            req.length = received;

            // Setup buffer
            rx_ptr = req.data;
            rx_len = 0;

            if (received == 0) state = SERIAL_TP_RCVD_TERMINATOR;
            else state = SERIAL_TP_RCVD_DATA;

            break;
        }

        case SERIAL_TP_RCVD_DATA:
        {
            rx_len++;

            if (rx_len <= SERIAL_TP_RX_BUFFER_LEN)
            {
                WRITE_PU8(rx_ptr++, received);
            }

            if (rx_len >= req.length)
            {
                state = SERIAL_TP_RCVD_TERMINATOR;
            }
            break;
        }

        case SERIAL_TP_RCVD_TERMINATOR:
        {
            if (received != SERIAL_TP_FRAME_TERMINATOR)
            {
                rsp.status = SERIAL_TP_RETCODE_EXPECTED_EOF;
            }
            else if (rx_len > SERIAL_TP_RX_BUFFER_LEN)
            {
                rsp.status = SERIAL_TP_RETCODE_BUFFER_OVERRUN;
            }
            else
            {
                Serial_TP_Callback(&req, &rsp);
            }

            Serial_SendResponse(&rsp);
            state = SERIAL_TP_RCVD_HEADER;

            break;
        }

        default: HALT;
    }
}
