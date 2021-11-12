#include "os.h"
#include "types.h"
#include "bits.h"
#include "hc165.h"
#include "gpio.h"

void hc165_init(hc165_t self)
{
    // TODO : set custom pinout once GPIO module is rewritten
    gpio_init(&self->serial, GPIO_PORT_C, 1, GPIO_INPUT_HIGH_Z);
    gpio_init(&self->latch,  GPIO_PORT_C, 2, GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_init(&self->clock,  GPIO_PORT_C, 3, GPIO_OUTPUT_ACTIVE_HIGH);
}

int hc165_read(hc165_t self, buffer_t buf, int len)
{
    bool ser = FALSE;

    gpio_set(&self->latch);

    for (int i = 0 ; i < len ; i++)
    {
        uint8_t *data = TYPECAST(buf++, uint8_t*);
        for (int j = 7 ; j >= 0 ; j--)
        {
            gpio_get(&self->serial, &ser);

            if (ser) SET_BIT(*data, j);

            gpio_set(&self->clock);
            os_wait(1);
            gpio_reset(&self->clock);
        }
    }

    gpio_reset(&self->latch);

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
