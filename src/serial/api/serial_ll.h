#ifndef SERIAL_LL_API_H__
#define SERIAL_LL_API_H__

#include "types.h"
#include "usart.h"

enum {
    SERIAL_LL_OK,
    SERIAL_LL_ERROR_INSTANCE,
};

#define C_END       "\33[0m"
#define C_BOLD      "\33[1m"
#define C_ITALIC    "\33[3m"
#define C_URL       "\33[4m"
#define C_BLINK     "\33[5m"
#define C_BLINK2    "\33[6m"
#define C_SELECTED  "\33[7m"

#define C_BLACK     "\33[30m"
#define C_RED       "\33[31m"
#define C_GREEN     "\33[32m"
#define C_YELLOW    "\33[33m"
#define C_BLUE      "\33[34m"
#define C_VIOLET    "\33[35m"
#define C_BEIGE     "\33[36m"
#define C_WHITE     "\33[37m"

#define C_BLACKBG   "\33[40m"
#define C_REDBG     "\33[41m"
#define C_GREENBG   "\33[42m"
#define C_YELLOWBG  "\33[43m"
#define C_BLUEBG    "\33[44m"
#define C_VIOLETBG  "\33[45m"
#define C_BEIGEBG   "\33[46m"
#define C_WHITEBG   "\33[47m"

#define C_GREY      "\33[90m"
#define C_RED2      "\33[91m"
#define C_GREEN2    "\33[92m"
#define C_YELLOW2   "\33[93m"
#define C_BLUE2     "\33[94m"
#define C_VIOLET2   "\33[95m"
#define C_BEIGE2    "\33[96m"
#define C_WHITE2    "\33[97m"

#define C_GREYBG    "\33[100m"
#define C_REDBG2    "\33[101m"
#define C_GREENBG2  "\33[102m"
#define C_YELLOWBG2 "\33[103m"
#define C_BLUEBG2   "\33[104m"
#define C_VIOLETBG2 "\33[105m"
#define C_BEIGEBG2  "\33[106m"
#define C_WHITEBG2  "\33[107m"

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

void serial_ll_reset(usart_t self);
void serial_ll_power_enable(usart_t self);
void serial_ll_init(usart_t self, uint32_t baudrate);
void serial_ll_wait_tx(usart_t self);
void serial_ll_wait_rx(usart_t self);
void serial_ll_write(usart_t self, uint8_t byte);
uint8_t serial_ll_read(usart_t usart);

#endif // SERIAL_LL_API_H__
