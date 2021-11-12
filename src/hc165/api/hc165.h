#ifndef SRC_HC165_API_H_
#define SRC_HC165_API_H_

#include "gpio.h"
#include "types.h"

typedef struct hc165_prv_s {
    gpio_t latch;
    gpio_t clock;
    gpio_t serial;
} * hc165_t;

void hc165_init(hc165_t self);
int hc165_read(hc165_t self, buffer_t buf, int len);

uint8_t hc165_read_byte(hc165_t self);
uint16_t hc165_read_word(hc165_t self);
uint32_t hc165_read_dword(hc165_t self);

#endif /* SRC_HC165_API_H_ */
