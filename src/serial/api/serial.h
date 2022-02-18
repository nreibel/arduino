#ifndef __SERIAL_API_H__
#define __SERIAL_API_H__

#include "types.h"
#include "serial_cfg.h"

typedef volatile uint8_t * reg8_t;
typedef volatile uint16_t * reg16_t;

typedef struct serial_bus_s {
    reg8_t PRRx;
    reg8_t UCSRxA;
    reg8_t UCSRxB;
    reg8_t UCSRxC;
    reg8_t UDRx;
    reg16_t UBRRx;
    uint8_t PRUSARTx;
    uint8_t RXCx;
    uint8_t UDREx;
    uint8_t TXCIEx;
    uint8_t RXCIEx;
    uint8_t RXENx;
    uint8_t TXENx;
} const * serial_bus_t;

extern serial_bus_t SERIAL_BUS_0;

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

#if SERIAL_ASYNC_RX != OFF
extern void serial_rx_callback(serial_bus_t bus, const char *buffer, unsigned int length);
#endif

#if SERIAL_ASYNC_TX != OFF
bool serial_tx_ready(serial_bus_t bus);
void serial_write_async(serial_bus_t bus, const void * buffer, unsigned int length);
#endif

int serial_bus_init(serial_bus_t bus, uint32_t baudrate);

int serial_read_byte(serial_bus_t bus, uint8_t *chr);
int serial_read_bytes(serial_bus_t bus, void * buffer, unsigned int length);

int serial_write_byte(serial_bus_t bus, uint8_t chr);
int serial_write_bytes(serial_bus_t bus, const void * buffer, unsigned int length);

int serial_print(serial_bus_t bus, const char * string);
int serial_println(serial_bus_t bus, const char * string);
int serial_new_line(serial_bus_t bus);

int serial_print_P(serial_bus_t bus, const __flash char * string);
int serial_println_P(serial_bus_t bus, const __flash char * string);

#endif /* __SERIAL_API_H__ */
