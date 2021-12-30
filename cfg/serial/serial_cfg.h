#ifndef __SERIAL_CFG_H__
#define __SERIAL_CFG_H__

#define SERIAL_ASYNC_RX OFF
#define SERIAL_ASYNC_TX ON

// Use Enter key as line separator, good for user input
#define SERIAL_LINE_TERMINATOR 0x0D

// Should be big enough to accomodate the biggest message to be received
#define SERIAL_RECEIVE_BUFFER_LENGTH 32

#endif /* __SERIAL_CFG_H__ */
