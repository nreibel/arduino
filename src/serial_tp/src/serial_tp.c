#include "serial.h"
#include "usart.h"
#include "serial_tp.h"
#include "serial_tp_cfg.h"
#include "types.h"
#include "bits.h"

/*
 * Public functions
 */

void serial_rx_callback(serial_callback_args_t * args)
{
    switch(args->event)
    {
        case SERIAL_EVENT_RX_LINE:
        {
            serial_tp_request_t * req = TYPECAST(args->buffer, serial_tp_request_t*);
            serial_tp_response_t  rsp = {
                .status = SERIAL_TP_RETCODE_FUNCTION_INVALID,
                .length = 0,
            };

            if ( req->header != SERIAL_TP_REQUEST_HEADER )
            {
                rsp.status = SERIAL_TP_RETCODE_INVALID_HEADER;
            }
            else if ( req->length != args->length - 4 )
            {
                rsp.status = SERIAL_TP_RETCODE_INVALID_DATA_LEN;
            }
            else
            {
                serial_tp_callback(args->instance, req, &rsp);
            }

            rsp.data[rsp.length] = SERIAL_TP_FRAME_TERMINATOR;

            serial_write_bytes(args->instance, &rsp, rsp.length+3);
            break;
        }

        default:
            break;
    }
}

void serial_tp_init(serial_t bus)
{
    static flstr_t ready = "READY";

    static uint8_t rx_buf[SERIAL_TP_REQUEST_BUFFER_LEN];

    serial_set_line_terminator(bus, SERIAL_TP_FRAME_TERMINATOR);
    serial_set_callback(bus, serial_rx_callback, rx_buf, sizeof(rx_buf));
    serial_print_P(bus, ready);
}
