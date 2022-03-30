#ifndef SERIAL_LL_API_H__
#define SERIAL_LL_API_H__

#include "types.h"
#include "usart.h"

#define COLOR(c)    "\33[" #c "m"
#define C_END       COLOR(0)
#define C_BOLD      COLOR(1)
#define C_ITALIC    COLOR(3)
#define C_URL       COLOR(4)
#define C_BLINK     COLOR(5)
#define C_BLINK2    COLOR(6)
#define C_SELECTED  COLOR(7)

#define C_BLACK     COLOR(30)
#define C_RED       COLOR(31)
#define C_GREEN     COLOR(32)
#define C_YELLOW    COLOR(33)
#define C_BLUE      COLOR(34)
#define C_VIOLET    COLOR(35)
#define C_BEIGE     COLOR(36)
#define C_WHITE     COLOR(37)

#define C_BLACKBG   COLOR(40)
#define C_REDBG     COLOR(41)
#define C_GREENBG   COLOR(42)
#define C_YELLOWBG  COLOR(43)
#define C_BLUEBG    COLOR(44)
#define C_VIOLETBG  COLOR(45)
#define C_BEIGEBG   COLOR(46)
#define C_WHITEBG   COLOR(47)

#define C_GREY      COLOR(90)
#define C_RED2      COLOR(91)
#define C_GREEN2    COLOR(92)
#define C_YELLOW2   COLOR(93)
#define C_BLUE2     COLOR(94)
#define C_VIOLET2   COLOR(95)
#define C_BEIGE2    COLOR(96)
#define C_WHITE2    COLOR(97)

#define C_GREYBG    COLOR(100)
#define C_REDBG2    COLOR(101)
#define C_GREENBG2  COLOR(102)
#define C_YELLOWBG2 COLOR(103)
#define C_BLUEBG2   COLOR(104)
#define C_VIOLETBG2 COLOR(105)
#define C_BEIGEBG2  COLOR(106)
#define C_WHITEBG2  COLOR(107)

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
    ucsra_t  ucsra;
    ucsrb_t  ucsrb;
    ucsrc_t  ucsrc;
    uint8_t  __reserved;
    uint16_t ubrr;
    uint8_t  udr;
} volatile * mem_usart_t;

extern void serial_tx_irq_handler(usart_t self);
extern void serial_rx_irq_handler(usart_t self);

void serial_ll_init(usart_t self, uint32_t baudrate);
void serial_ll_power_enable(usart_t self);
void serial_ll_reset(usart_t self);
void serial_ll_set_tx_irq(usart_t self, bool enabled);
void serial_ll_set_rx_irq(usart_t self, bool enabled);
void serial_ll_wait_tx(usart_t self);
void serial_ll_wait_rx(usart_t self);
void serial_ll_write(usart_t self, uint8_t byte);
uint8_t serial_ll_read(usart_t self);

#endif // SERIAL_LL_API_H__
