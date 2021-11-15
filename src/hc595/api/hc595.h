#ifndef SRC_hc595_API_H_
#define SRC_hc595_API_H_

#include "types.h"
#include "gpio.h"
#include "os_cfg.h"

typedef struct hc595_prv_s {
    gpio_t serial;
    gpio_t clock;
    gpio_t latch;
} * hc595_t;

#if OS_MALLOC
hc595_t hc595_create(gpio_t serial, gpio_t clock, gpio_t latch);
void hc595_destroy(hc595_t self);
#endif // OS_MALLOC

void hc595_init(hc595_t self, gpio_t serial, gpio_t clock, gpio_t latch);
void hc595_write(hc595_t self, buffer_t data, int len);

void hc595_write_byte(hc595_t self, uint8_t val);
void hc595_write_word(hc595_t self, uint16_t val);
void hc595_write_dword(hc595_t self, uint32_t val);

#endif /* SRC_hc595_API_H_ */
