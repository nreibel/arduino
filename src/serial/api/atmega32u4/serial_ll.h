#ifndef SERIAL_LL_API_H__
#define SERIAL_LL_API_H__

#include "types.h"

typedef union {
    struct {
        uint8_t MPCM : 1;
        uint8_t U2X  : 1;
        uint8_t UPE  : 1;
        uint8_t DOR  : 1;
        uint8_t FE   : 1;
        uint8_t UDRE : 1;
        uint8_t TXC  : 1;
        uint8_t RXC  : 1;
    } bits;
    uint8_t reg;
} ucsra_t;

typedef union {
    struct {
        uint8_t TXB8  : 1;
        uint8_t RXB8  : 1;
        uint8_t UCSZ2 : 1;
        uint8_t TXEN  : 1;
        uint8_t RXEN  : 1;
        uint8_t UDRIE : 1;
        uint8_t TXCIE : 1;
        uint8_t RXCIE : 1;
    } bits;
    uint8_t reg;
} ucsrb_t;

typedef union {
    struct {
        uint8_t UCPOL : 1;
        uint8_t UCSZ  : 2;
        uint8_t USBS  : 1;
        uint8_t UPM   : 2;
        uint8_t UMSEL : 2;
    } bits;
    uint8_t reg;
} ucsrc_t;


typedef union {
    struct {
        uint8_t RTSEN : 1;
        uint8_t CTSEN : 1;
        uint8_t __pad : 6;
    } bits;
    uint8_t reg;
} ucsrd_t;

typedef struct {
    volatile ucsra_t  UCSRA;
    volatile ucsrb_t  UCSRB;
    volatile ucsrc_t  UCSRC;
    volatile ucsrd_t  UCSRD;
    volatile uint16_t UBRR;
    volatile uint8_t  UDR;
} * usart_t;

#define NUMBER_OF_USART 1
#define USART1 TYPECAST(0xC8, const usart_t)

#endif // SERIAL_LL_API_H__
