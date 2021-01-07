#ifndef SRC_SERIAL_CFG_SERIAL_CFG_H_
#define SRC_SERIAL_CFG_SERIAL_CFG_H_

#include "types.h"

#define SERIAL_BAUD_RATE 19200U

#define SERIAL_ASYNC_RX ON
#define SERIAL_ASYNC_TX OFF

#if SERIAL_ASYNC_RX == ON
    // Use Enter key as line separator, good for user input
    #define SERIAL_LINE_TERMINATOR       0x0D

    // Should be big enough to accomodate the biggest message to be received
    #define SERIAL_RECEIVE_BUFFER_LENGTH 16
#endif

#endif /* SRC_SERIAL_CFG_SERIAL_CFG_H_ */
