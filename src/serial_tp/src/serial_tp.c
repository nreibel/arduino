#include "serial_tp.h"
#include "serial_tp_cfg.h"
#include "types.h"
#include "bits.h"
#include "st7735.h"

static void Serial_SendResponse(uint8_t status, Serial_TP_Response *rsp);

typedef enum {
    SERIAL_TP_RCVD_HEADER,
    SERIAL_TP_RCVD_FUNCTION,
    SERIAL_TP_RCVD_ADDRESS,
    SERIAL_TP_RCVD_LENGTH,
    SERIAL_TP_RCVD_DATA,
    SERIAL_TP_RCVD_TERMINATOR,
} serial_tp_state_t;

static void Serial_SendResponse(uint8_t status, Serial_TP_Response *rsp)
{
    Serial_WriteByte(status);

    if (rsp == NULL_PTR)
    {
        Serial_WriteByte(0);
    }
    else
    {
        Serial_WriteByte(rsp->length);
        Serial_WriteBytes(rsp->data, rsp->length);
    }

    Serial_WriteByte(SERIAL_TP_FRAME_TERMINATOR);
}

void Serial_RxCallback(uint8_t received)
{
    static Serial_TP_Request req = {0};

    static uint8_t tx_buffer[SERIAL_TP_TX_BUFFER_LEN] = {0};
    static uint8_t rx_buffer[SERIAL_TP_RX_BUFFER_LEN] = {0};

    static uint8_t *rx_ptr = NULL_PTR;
    static int rx_size = 0;

    static serial_tp_state_t state = SERIAL_TP_RCVD_HEADER;

    switch(state)
    {
        case SERIAL_TP_RCVD_HEADER:
        {
            if (received == SERIAL_TP_REQUEST_HEADER) state = SERIAL_TP_RCVD_FUNCTION;
            else Serial_SendResponse(SERIAL_TP_RETCODE_INVALID_HEADER, NULL_PTR);
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

            rx_ptr = rx_buffer;
            rx_size = 0;

            if (req.length == 0) state = SERIAL_TP_RCVD_TERMINATOR;
            else state = SERIAL_TP_RCVD_DATA;

            break;
        }

        case SERIAL_TP_RCVD_DATA:
        {
            rx_size++;

            if (rx_size <= SERIAL_TP_RX_BUFFER_LEN)
            {
                WRITE_PU8(rx_ptr++, received);
            }

            if (rx_size >= req.length)
            {
                state = SERIAL_TP_RCVD_TERMINATOR;
            }
            break;
        }

        case SERIAL_TP_RCVD_TERMINATOR:
        {
            if (received == SERIAL_TP_FRAME_TERMINATOR)
            {
                if (rx_size > SERIAL_TP_RX_BUFFER_LEN)
                {
                    Serial_SendResponse(SERIAL_TP_RETCODE_BUFFER_OVERRUN, NULL_PTR);
                }
                else
                {
                    req.data = rx_buffer;

                    Serial_TP_Response rsp = {tx_buffer, 0};
                    uint8_t status = Serial_TP_Callback(&req, &rsp);
                    Serial_SendResponse(status, &rsp);
                }
            }
            else
            {
                Serial_SendResponse(SERIAL_TP_RETCODE_EXPECTED_EOF, NULL_PTR);
            }

            state = SERIAL_TP_RCVD_HEADER;

            break;
        }

        default: HALT;
    }
}
