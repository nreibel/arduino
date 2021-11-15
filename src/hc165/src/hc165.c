#include "os.h"
#include "types.h"
#include "bits.h"
#include "hc165.h"
#include "gpio.h"

#ifndef HC165_EDGE_DELAY
#define HC165_EDGE_DELAY 1
#endif // HC165_EDGE_DELAY

#if OS_MALLOC
hc165_t hc165_create(gpio_t serial, gpio_t clock, gpio_t latch)
{
    hc165_t instance = os_malloc(sizeof(*instance));
    if (instance != NULL_PTR) hc165_init(instance, serial, clock, latch);
    return instance;
}

void hc165_destroy(hc165_t self)
{
    os_free(self);
}
#endif // OS_MALLOC

void hc165_init(hc165_t self, gpio_t serial, gpio_t clock, gpio_t latch)
{
    gpio_set_data_direction(serial, GPIO_INPUT_HIGH_Z);
    gpio_set_data_direction(clock,  GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_set_data_direction(latch,  GPIO_OUTPUT_ACTIVE_HIGH);

    self->serial = serial;
    self->clock = clock;
    self->latch = latch;
}

int hc165_read(hc165_t self, buffer_t buf, int len)
{
    bool ser = FALSE;

    gpio_set(self->latch);

    for (int i = 0 ; i < len ; i++)
    {
        uint8_t *data = TYPECAST(buf++, uint8_t*);
        for (int j = 7 ; j >= 0 ; j--)
        {
            gpio_get(self->serial, &ser);

            if (ser) SET_BIT(*data, j);

            gpio_set(self->clock);
            os_wait(HC165_EDGE_DELAY);
            gpio_reset(self->clock);
        }
    }

    gpio_reset(self->latch);

    return len;
}

uint8_t hc165_read_byte(hc165_t self)
{
    uint8_t data = 0;
    hc165_read(self, &data, 1);
    return data;
}

uint16_t hc165_read_word(hc165_t self)
{
    uint16_t data = 0;
    hc165_read(self, &data, 2);
    return data;
}

uint32_t hc165_read_dword(hc165_t self)
{
    uint32_t data = 0;
    hc165_read(self, &data, 4);
    return data;
}
