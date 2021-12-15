#ifndef __SERIAL_TP_API_H__
#define __SERIAL_TP_API_H__

#include "types.h"
#include "serial.h"

#if SERIAL_ASYNC_RX != ON
#error "Serial TP needs SERIAL_ASYNC_RX enabled!"
#endif

typedef enum {
    // Generic errors
    SERIAL_TP_RETCODE_OK = 0x0,
    SERIAL_TP_RETCODE_ERROR,
    SERIAL_TP_RETCODE_OUT_OF_BOUNDS,

    // Protol errors
    SERIAL_TP_RETCODE_INVALID_HEADER = 0x10,
    SERIAL_TP_RETCODE_INVALID_DATA_LEN,

    // User callback errors
    SERIAL_TP_RETCODE_FUNCTION_INVALID = 0x20,
    SERIAL_TP_RETCODE_ADDRESS_INVALID,
    SERIAL_TP_RETCODE_DATA_INVALID
} serial_tp_error_t;


typedef struct {
    uint8_t header;
    uint8_t function;
    uint8_t address;
    uint8_t data_len;
    uint8_t data[];
} serial_tp_request;

typedef uint8_t (*serial_tp_callback)(serial_tp_request *req, void *rsp_data, uint8_t *rsp_length);

void serial_tp_init(serial_bus_t bus, serial_tp_callback cbk);

#endif /* __SERIAL_TP_API_H__ */
