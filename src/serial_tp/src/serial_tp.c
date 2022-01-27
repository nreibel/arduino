#include "serial_tp.h"
#include "serial_tp_cfg.h"
#include "types.h"
#include "bits.h"

/*
 * Private types
 */

struct rsp_s {
    uint8_t status;                              /* Status code                                */
    uint8_t length;                              /* Data length, in bytes                      */
    uint8_t data[SERIAL_TP_RESPONSE_BUFFER_LEN]; /* Data buffer                                */
    uint8_t terminator;                          /* One extra byte in case data buffer is full */
};

/*
 * Private data
 */

static struct rsp_s rsp;
static const __flash char STR_READY[] = "READY";

/*
 * Public functions
 */

void serial_rx_callback(serial_bus_t bus, const char *data, unsigned int len)
{
    serial_tp_request_t * req = TYPECAST(data, serial_tp_request_t*);

    rsp.length = 0;

    if ( req->header != SERIAL_TP_REQUEST_HEADER )
    {
        rsp.status = SERIAL_TP_RETCODE_INVALID_HEADER;
    }
    else if ( req->data_len != len - sizeof(serial_tp_request_t) - 1 )
    {
        rsp.status = SERIAL_TP_RETCODE_INVALID_DATA_LEN;
    }
    else
    {
        rsp.status = serial_tp_callback(bus, req, rsp.data, &rsp.length);
    }

    rsp.data[rsp.length] = SERIAL_TP_FRAME_TERMINATOR;

    #if SERIAL_ASYNC_TX == ON
    serial_write_async(bus, &rsp, rsp.length + 3);
    #else
    serial_write_bytes(bus, &rsp, rsp.length + 3);
    #endif // SERIAL_ASYNC_TX
}

void serial_tp_init(serial_bus_t bus)
{
    serial_print_P(bus, STR_READY);
}
