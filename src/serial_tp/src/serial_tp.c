#include "serial_tp.h"
#include "serial_tp_cfg.h"
#include "types.h"
#include "bits.h"

/*
 * Private data
 */

static uint8_t tx_buffer[SERIAL_TP_RESPONSE_BUFFER_LEN] = {0};
static serial_tp_callback callbacks[NUMBER_OF_SERIAL_BUSES] =  {0};

/*
 * Public functions
 */

void serial_rx_cbk(serial_bus_t bus, const char *rx_buffer, int rx_length)
{
    serial_tp_request *req = TYPECAST(rx_buffer, serial_tp_request*);

    uint8_t status = SERIAL_TP_RETCODE_ERROR;
    int tx_length = 0;

    if ( req->header != SERIAL_TP_REQUEST_HEADER )
    {
        // Check request header
        status = SERIAL_TP_RETCODE_INVALID_HEADER;
    }
    else if ( req->data_len != rx_length - sizeof(serial_tp_request) - 1 )
    {
        // Check data length
        status = SERIAL_TP_RETCODE_INVALID_DATA_LEN;
    }
    else
    {
        // User callback
        status = callbacks[bus](req, tx_buffer, &tx_length);
    }

    // Send response
    serial_write_byte(bus, status);
    serial_write_byte(bus, tx_length);
    serial_write_bytes(bus, tx_buffer, tx_length);
    serial_write_byte(bus, SERIAL_TP_FRAME_TERMINATOR);
}

void serial_tp_init(serial_bus_t bus, serial_tp_callback cbk)
{
    serial_set_rx_callback(bus, serial_rx_cbk);
    callbacks[bus] = cbk;
}
