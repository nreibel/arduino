#include "serial_tp.h"
#include "serial_tp_cfg.h"
#include "types.h"
#include "bits.h"

static uint8_t buffer[SERIAL_TP_RESPONSE_BUFFER_LEN] = {0};
static serial_tp_callback callbacks[NUMBER_OF_SERIAL_BUSES] =  {0};

void serial_rx_cbk(serial_bus_t bus, const char *rx_buffer, int length)
{
    serial_tp_request *req = TYPECAST(rx_buffer, serial_tp_request*);
    serial_tp_response rsp = {SERIAL_TP_RETCODE_ERROR, 0, buffer};

    if ( req->header != SERIAL_TP_REQUEST_HEADER )
    {
        // Check request header
        rsp.status = SERIAL_TP_RETCODE_INVALID_HEADER;
    }
    else if ( req->data_len != length - sizeof(serial_tp_request) - 1 )
    {
        // Check data length
        rsp.status = SERIAL_TP_RETCODE_INVALID_DATA_LEN;
    }
    else
    {
        // User callback
        callbacks[bus](req, &rsp);
    }

    // Send response
    serial_write_bytes(bus, &rsp, 2);
    serial_write_bytes(bus, rsp.data, rsp.data_len);
    serial_write_byte(bus, SERIAL_TP_FRAME_TERMINATOR);
}

void serial_tp_init(serial_bus_t bus, serial_tp_callback cbk)
{
    serial_set_rx_callback(bus, serial_rx_cbk);
    callbacks[bus] = cbk;
}
