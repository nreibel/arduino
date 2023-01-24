#ifndef __SERIAL_API_H__
#define __SERIAL_API_H__

#include <stdarg.h>
#include "types.h"
#include "os_cfg.h"
#include "serial_ll.h"

enum {
    SERIAL_OK,
    SERIAL_ERR_BUSY,
};

typedef enum {
    SERIAL_EVENT_RX_CHAR,
    SERIAL_EVENT_RX_LINE,
    SERIAL_EVENT_RX_OVERFLOW,
    SERIAL_EVENT_TX_COMPLETE,
} serial_event_t;

typedef struct serial_prv_s * serial_t;

typedef void (*serial_callback) (serial_t self, serial_event_t event, const uint8_t * buffer, unsigned int length);

struct serial_prv_s
{
    usart_t             dev;
    char                ter;
    serial_callback     cbk;
    volatile struct {
        uint8_t *       buf;
        unsigned int    len;
        unsigned int    cnt;
    } rx;
    volatile struct {
        const uint8_t * buf;
        unsigned int    len;
        unsigned int    cnt;
        bool            bsy;
    } tx;
};

#if OS_MALLOC
serial_t serial_create(void);
void serial_destroy(serial_t self);
#endif // OS_MALLOC

int     serial_init                 (serial_t self, usart_t usart, uint32_t baudrate);
int     serial_set_callback         (serial_t self, serial_callback callback, void * buffer, unsigned int length);
int     serial_set_line_terminator  (serial_t self, char terminator);
int     serial_read_byte            (serial_t self, uint8_t *chr);
int     serial_read_bytes           (serial_t self, void * buffer, unsigned int length);
int     serial_write_byte           (serial_t self, uint8_t chr);
int     serial_write_bytes          (serial_t self, const void * buffer, unsigned int length);
int     serial_write_async          (serial_t self, const void * buffer, unsigned int length);

int     serial_print                (serial_t self, const char * string);
int     serial_printf               (serial_t self, void * buffer, unsigned int length, const char * fmt, ...);
int     serial_printf_async         (serial_t self, void * buffer, unsigned int length, const char * fmt, ...);

int     serial_print_P              (serial_t self, flstr_t string);

// TODO int serial_printf_P(serial_t self, void * buffer, unsigned int length, const char * fmt, ...);
// TODO int serial_printf_async_P(serial_t self, void * buffer, unsigned int length, const char * fmt, ...);

#endif /* __SERIAL_API_H__ */
