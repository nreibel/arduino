#ifndef __SERIAL_API_H__
#define __SERIAL_API_H__

#include "types.h"
#include "serial_ll.h"
#include "serial_cfg.h"

enum {
    SERIAL_OK,
    SERIAL_ERROR_INSTANCE,
    SERIAL_ERROR_BUSY,
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

#if SERIAL_ASYNC_RX
extern void serial_rx_callback(usart_t usart, const char *buffer, unsigned int length);
#endif

#if SERIAL_ASYNC_TX
bool serial_tx_ready(usart_t usart);
int serial_write_async(usart_t usart, const void * buffer, unsigned int length);
#endif

int serial_init(usart_t usart, uint32_t baudrate);

int serial_read_byte(usart_t usart, uint8_t *chr);
int serial_read_bytes(usart_t usart, void * buffer, unsigned int length);

int serial_write_byte(usart_t usart, uint8_t chr);
int serial_write_bytes(usart_t usart, const void * buffer, unsigned int length);

int serial_print(usart_t usart, const char * string);
int serial_println(usart_t usart, const char * string);
int serial_new_line(usart_t usart);

int serial_print_P(usart_t usart, const __flash char * string);
int serial_println_P(usart_t usart, const __flash char * string);

#endif /* __SERIAL_API_H__ */
