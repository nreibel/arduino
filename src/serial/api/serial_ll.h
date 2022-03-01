#ifndef SERIAL_LL_API_H__
#define SERIAL_LL_API_H__

#include "types.h"
#include "usart.h"

typedef union {
    struct {
        uint8_t mpcm : 1;
        uint8_t u2x  : 1;
        uint8_t upe  : 1;
        uint8_t dor  : 1;
        uint8_t fe   : 1;
        uint8_t udre : 1;
        uint8_t txc  : 1;
        uint8_t rxc  : 1;
    } bits;
    uint8_t reg;
} ucsra_t;

typedef union {
    struct {
        uint8_t txb8  : 1;
        uint8_t rxb8  : 1;
        uint8_t ucsz2 : 1;
        uint8_t txen  : 1;
        uint8_t rxen  : 1;
        uint8_t udrie : 1;
        uint8_t txcie : 1;
        uint8_t rxcie : 1;
    } bits;
    uint8_t reg;
} ucsrb_t;

typedef union {
    struct {
        uint8_t ucpol : 1;
        uint8_t ucsz  : 2;
        uint8_t usbs  : 1;
        uint8_t upm   : 2;
        uint8_t umsel : 2;
    } bits;
    uint8_t reg;
} ucsrc_t;

typedef struct {
    volatile ucsra_t  ucsra;
    volatile ucsrb_t  ucsrb;
    volatile ucsrc_t  ucsrc;
    volatile uint8_t  __reserved;
    volatile uint16_t ubrr;
    volatile uint8_t  udr;
} * mem_usart_t;

void serial_ll_init(usart_t self, uint32_t baudrate);
void serial_ll_wait_for_tx_ready(usart_t self);
void serial_ll_wait_for_rx_ready(usart_t self);
void serial_ll_write_byte(usart_t self, uint8_t byte);
uint8_t serial_ll_read_byte(usart_t self);

#endif // SERIAL_LL_API_H__
