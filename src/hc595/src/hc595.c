#include "hc595.h"
#include "bits.h"
#include "gpio.h"
#include "os.h"

#define HC595_EDGE_DELAY 1 // in µs

#if OS_MALLOC
hc595_t hc595_create()
{
    hc595_t instance = os_malloc(sizeof(*instance));
    return instance;
}

void hc595_destroy(hc595_t self)
{
    if (self != NULL_PTR)
        os_free(self);
}
#endif // OS_MALLOC

void hc595_init(hc595_t self, gpio_t serial, gpio_t clock, gpio_t latch)
{
    gpio_configure(serial, GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_configure(clock,  GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_configure(latch,  GPIO_OUTPUT_ACTIVE_HIGH);

    self->serial = serial;
    self->clock = clock;
    self->latch = latch;
}

int hc595_write(hc595_t self, void * data, int len)
{
    uint8_t * bytes = data;

    for (int i = len-1 ; i >= 0 ; i--)
    {
        for (int j = 7 ; j >= 0 ; j--)
        {
            bool st = CHECK_BIT(bytes[i], j);

            if (st) gpio_set(self->serial);
            else gpio_reset(self->serial);

            gpio_set(self->clock);
            os_wait_us(HC595_EDGE_DELAY);
            gpio_reset(self->clock);
        }
    }

    gpio_set(self->latch);
    os_wait_us(HC595_EDGE_DELAY);
    gpio_reset(self->latch);

    return len;
}

int hc595_write_byte(hc595_t self, uint8_t val)
{
    return hc595_write(self, &val, 1);
}

int hc595_write_word(hc595_t self, uint16_t val)
{
    return hc595_write(self, &val, 2);
}

int hc595_write_dword(hc595_t self, uint32_t val)
{
    return hc595_write(self, &val, 4);
}
