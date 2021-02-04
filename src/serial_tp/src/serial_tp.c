#include "serial_tp.h"
#include "serial_tp_cfg.h"
#include "types.h"
#include "bits.h"

static uint8_t buffer[SERIAL_TP_RESPONSE_BUFFER_LEN] = {0};

void Serial_TP_Init()
{
    // Nothing to do
}

void Serial_RxCallback(const char *rx_buffer, int length)
{
    Serial_TP_Request *req = TYPECAST(rx_buffer, Serial_TP_Request*);
    Serial_TP_Response rsp = {SERIAL_TP_RETCODE_ERROR, 0, buffer};

    if ( req->header != SERIAL_TP_REQUEST_HEADER )
    {
        // Check request header
        rsp.status = SERIAL_TP_RETCODE_INVALID_HEADER;
    }
    else if ( req->data_len != length - sizeof(Serial_TP_Request) - 1 )
    {
        // Check data length
        rsp.status = SERIAL_TP_RETCODE_INVALID_DATA_LEN;
    }
    else
    {
        // User callback
        Serial_TP_Callback(req, &rsp);
    }

    // Send response
    Serial_WriteBytes(&rsp, 2);
    Serial_WriteBytes(rsp.data, rsp.data_len);
    Serial_WriteByte(SERIAL_TP_FRAME_TERMINATOR);
}
