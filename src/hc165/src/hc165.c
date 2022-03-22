#include "os.h"
#include "os_cfg.h"
#include "os_mem.h"
#include "types.h"
#include "bits.h"
#include "hc165.h"
#include "gpio.h"

#define HC165_EDGE_DELAY 1 // in µs

#if OS_MALLOC
hc165_t hc165_create()
{
    hc165_t instance = os_malloc(sizeof(*instance));
    return instance;
}

void hc165_destroy(hc165_t self)
{
    if (self != NULL_PTR)
        os_free(self);
}
#endif // OS_MALLOC

void hc165_init(hc165_t self, gpio_t serial, gpio_t clock, gpio_t latch)
{
    gpio_set_data_direction(serial, GPIO_INPUT_HIGH_Z);
    gpio_set_data_direction(clock,  GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_set_data_direction(latch,  GPIO_OUTPUT_ACTIVE_LOW);

    self->serial = serial;
    self->clock = clock;
    self->latch = latch;
}

int hc165_read(hc165_t self, void * data, int len)
{
    uint8_t * bytes = data;

    gpio_set(self->latch);
    os_wait_us(HC165_EDGE_DELAY);
    gpio_reset(self->latch);

    for (int i = 0 ; i < len ; i++)
    {
        uint8_t byte = 0;

        for (int j = 7 ; j >= 0 ; j--)
        {
            bool ser = FALSE;
            gpio_get(self->serial, &ser);
            if (ser) SET_BIT(byte, j);

            gpio_set(self->clock);
            os_wait_us(HC165_EDGE_DELAY);
            gpio_reset(self->clock);
        }

        bytes[i] = byte;
    }

    return len;
}

int hc165_read_byte(hc165_t self, uint8_t * value)
{
    return hc165_read(self, value, 1);
}

int hc165_read_word(hc165_t self, uint16_t * value)
{
    return hc165_read(self, value, 2);
}

int hc165_read_dword(hc165_t self, uint32_t * value)
{
    return hc165_read(self, value, 4);
}
