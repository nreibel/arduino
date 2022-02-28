#ifndef USART_API_H__
#define USART_API_H__

#include "types.h"

typedef enum {
    USART1,
    NUMBER_OF_USART
} usart_t;

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


typedef union {
    struct {
        uint8_t rtsen : 1;
        uint8_t ctsen : 1;
        uint8_t __pad : 6;
    } bits;
    uint8_t reg;
} ucsrd_t;

typedef struct {
    volatile ucsra_t  ucsra;
    volatile ucsrb_t  ucsrb;
    volatile ucsrc_t  ucsrc;
    volatile ucsrd_t  ucsrd;
    volatile uint16_t ubrr;
    volatile uint8_t  udr;
} * mem_usart_t;

#endif // USART_API_H__
