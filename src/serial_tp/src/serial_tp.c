#include "serial_tp.h"
#include "serial_tp_cfg.h"
#include "types.h"
#include "bits.h"

/*
 * Private types
 */

typedef struct {
    uint8_t status;
    uint8_t length;
    uint8_t data[SERIAL_TP_RESPONSE_BUFFER_LEN+1];
} serial_tp_response;

/*
 * Private data
 */

static serial_tp_callback callbacks[NUMBER_OF_SERIAL_BUSES] =  {0};

/*
 * Public functions
 */

static void serial_rx_cbk(serial_bus_t bus, const char *rx_buffer, int rx_length)
{
    static serial_tp_response rsp;
    serial_tp_request *req = TYPECAST(rx_buffer, serial_tp_request*);

    if ( req->header != SERIAL_TP_REQUEST_HEADER )
    {
        rsp.length = 0;
        rsp.status = SERIAL_TP_RETCODE_INVALID_HEADER;
    }
    else if ( req->data_len != rx_length - sizeof(serial_tp_request) - 1 )
    {
        rsp.length = 0;
        rsp.status = SERIAL_TP_RETCODE_INVALID_DATA_LEN;
    }
    else
    {
        // User callback
        rsp.status = callbacks[bus](req, rsp.data, &rsp.length);
    }

    rsp.data[rsp.length] = SERIAL_TP_FRAME_TERMINATOR;

    #if SERIAL_ASYNC_TX == ON
    serial_write_async(bus, &rsp, rsp.length + 3);
    #else
    serial_write_bytes(bus, &rsp, rsp.length + 3);
    #endif // SERIAL_ASYNC_TX
}

void serial_tp_init(serial_bus_t bus, serial_tp_callback cbk)
{
    serial_set_rx_callback(bus, serial_rx_cbk);
    callbacks[bus] = cbk;
}
