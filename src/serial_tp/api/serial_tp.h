#ifndef SRC_SERIAL_TP_API_SERIAL_H_
#define SRC_SERIAL_TP_API_SERIAL_H_

#include "types.h"
#include "serial.h"
#include "serial_tp_cfg.h"

#if !defined(SRC_SERIAL_API_SERIAL_H_)
#error "Serial TP needs module Serial"
#endif

#if SERIAL_ASYNC_RX != ON
#error "Serial TP needs SERIAL_ASYNC_RX enabled!"
#endif

#define SERIAL_TP_REQUEST_HEADER            0xA0

#define SERIAL_TP_RETCODE_OK                0x00
#define SERIAL_TP_RETCODE_ERROR             0x01

// Protol errors
#define SERIAL_TP_RETCODE_INVALID_HEADER     0x10
#define SERIAL_TP_RETCODE_EXPECTED_EOF       0x11
#define SERIAL_TP_RETCODE_BUFFER_OVERRUN     0x12

// User callback errors
#define SERIAL_TP_RETCODE_FUNCTION_INVALID  0x20
#define SERIAL_TP_RETCODE_ADDRESS_INVALID   0x21
#define SERIAL_TP_RETCODE_DATA_INVALID      0x22

typedef struct {
    uint8_t function;
    uint8_t address;
    uint8_t length;
    uint8_t data[SERIAL_TP_RX_BUFFER_LEN];
} Serial_TP_Request;

typedef struct {
    uint8_t status;
    uint8_t length;
    uint8_t data[SERIAL_TP_TX_BUFFER_LEN];
} Serial_TP_Response;

void Serial_TP_Callback(Serial_TP_Request *req, Serial_TP_Response *rsp);

#endif /* SRC_SERIAL_TP_API_SERIAL_H_ */
