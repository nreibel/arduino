#ifndef SRC_HC165_API_H_
#define SRC_HC165_API_H_

#include "gpio.h"
#include "types.h"
#include "os_cfg.h"

typedef struct hc165_prv_s {
    gpio_t serial;
    gpio_t clock;
    gpio_t latch;
} * hc165_t;

#if OS_MALLOC
hc165_t hc165_create();
void hc165_destroy(hc165_t self);
#endif // OS_MALLOC

void hc165_init(hc165_t self, gpio_t serial, gpio_t clock, gpio_t latch);
int hc165_read(hc165_t self, void * data, int len);

int hc165_read_byte(hc165_t self, uint8_t * value);
int hc165_read_word(hc165_t self, uint16_t * value);
int hc165_read_dword(hc165_t self, uint32_t * value);

#endif /* SRC_HC165_API_H_ */
