#ifndef __SERIAL_TP_API_H__
#define __SERIAL_TP_API_H__

#include "types.h"
#include "serial.h"

#if SERIAL_ASYNC_RX != ON
#error "Serial TP needs SERIAL_ASYNC_RX enabled!"
#endif

#define SERIAL_TP_REQUEST_HEADER            0xA0

#define SERIAL_TP_RETCODE_OK                0x00
#define SERIAL_TP_RETCODE_ERROR             0x01

// Protol errors
#define SERIAL_TP_RETCODE_INVALID_HEADER    0x10
#define SERIAL_TP_RETCODE_INVALID_DATA_LEN  0x11

// User callback errors
#define SERIAL_TP_RETCODE_FUNCTION_INVALID  0x20
#define SERIAL_TP_RETCODE_ADDRESS_INVALID   0x21
#define SERIAL_TP_RETCODE_DATA_INVALID      0x22


typedef struct {
    uint8_t header;
    uint8_t function;
    uint8_t address;
    uint8_t data_len;
    uint8_t data[];
} serial_tp_request;

typedef struct {
    uint8_t status;
    uint8_t data_len;
    void   *data;
} serial_tp_response;

typedef void (*serial_tp_callback)(serial_tp_request *req, serial_tp_response *rsp);

void serial_tp_init(serial_bus_t bus, serial_tp_callback cbk);

#endif /* __SERIAL_TP_API_H__ */
