#include "hc595.h"
#include "bits.h"
#include "gpio.h"
#include "os.h"

#ifndef HC595_EDGE_DELAY
#define HC595_EDGE_DELAY 0
#endif // HC595_EDGE_DELAY

void hc595_init(hc595_t self)
{
    // TODO : set custom pinout once GPIO module is rewritten
    gpio_init(&self->serial, GPIO_PORT_D, 2, GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_init(&self->latch,  GPIO_PORT_D, 3, GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_init(&self->clock,  GPIO_PORT_D, 4, GPIO_OUTPUT_ACTIVE_HIGH);
}

void hc595_write(hc595_t self, buffer_t data, int len)
{
    for (int i = len ; i > 0 ; i--)
    {
        uint8_t val = READ_PU8(data+i-1);
        for (int j = 7 ; j >= 0 ; j--)
        {
            bool st = IS_SET_BIT(val, j);
            if (st) gpio_set(&self->serial);
            else gpio_reset(&self->serial);

            gpio_set(&self->clock);
            os_wait(HC595_EDGE_DELAY);
            gpio_reset(&self->clock);
        }
    }

    gpio_set(&self->latch);
    os_wait(HC595_EDGE_DELAY);
    gpio_reset(&self->latch);
}

void hc595_write_byte(hc595_t self, uint8_t val)
{
    hc595_write(self, &val, 1);
}

void hc595_write_word(hc595_t self, uint16_t val)
{
    hc595_write(self, &val, 2);
}

void hc595_write_dword(hc595_t self, uint32_t val)
{
    hc595_write(self, &val, 4);
}
