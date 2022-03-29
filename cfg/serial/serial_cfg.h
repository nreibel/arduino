#ifndef __SERIAL_CFG_H__
#define __SERIAL_CFG_H__

#define SERIAL_ASYNC_RX ON
#define SERIAL_ASYNC_TX OFF

// Use Enter key as line separator, good for user input
#define SERIAL_LINE_TERMINATOR 0x0D

// Should be big enough to accomodate the biggest message to be received
#define SERIAL_RECEIVE_BUFFER_LENGTH 32

// Name of the deferred task to use to handle RX callbacks
// If not defined, RX callbacks are called from the interrupt handler
// TODO Can only handle one serial device for now
// #define SERIAL_ASYNC_TX_DEFERRED_TASK DEFFERED_TASK_RX_CBK

#endif /* __SERIAL_CFG_H__ */
