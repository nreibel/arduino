#ifndef __SERIAL_TP_API_H__
#define __SERIAL_TP_API_H__

#include "types.h"
#include "serial.h"
#include "serial_tp_cfg.h"

typedef enum {
    // Generic errors
    SERIAL_TP_RETCODE_OK = 0x0,
    SERIAL_TP_RETCODE_ERROR,
    SERIAL_TP_RETCODE_OUT_OF_BOUNDS,

    // Protol errors
    SERIAL_TP_RETCODE_PROTOCOL = 0x10,
    SERIAL_TP_RETCODE_INVALID_HEADER,
    SERIAL_TP_RETCODE_INVALID_DATA_LEN,

    // User callback errors
    SERIAL_TP_RETCODE_FUNCTION_INVALID = 0x20,
    SERIAL_TP_RETCODE_ADDRESS_INVALID,
    SERIAL_TP_RETCODE_DATA_INVALID,

    // User App errors start here
    SERIAL_TP_RETCODE_USER_APP = 0x80 // This error code should not be used directly
} serial_tp_error_t;


typedef struct {
    uint8_t header;
    uint8_t function;
    uint8_t address;
    uint8_t length;
    uint8_t data[];
} serial_tp_request_t;

typedef struct {
    uint8_t status;                              /* Status code                                */
    uint8_t length;                              /* Data length, in bytes                      */
    uint8_t data[SERIAL_TP_RESPONSE_BUFFER_LEN]; /* Data buffer                                */
} serial_tp_response_t;

extern void serial_tp_callback(serial_t bus, const serial_tp_request_t * req, serial_tp_response_t * rsp);

void serial_tp_init(serial_t bus);

#endif /* __SERIAL_TP_API_H__ */
