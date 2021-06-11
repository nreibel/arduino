#ifndef __GPIO_H__
#define __GPIO_H__

#include "types.h"

typedef enum
{
    GPIO_EDGE_LOW,
    GPIO_EDGE_FALLING,
    GPIO_EDGE_RISING,
    GPIO_EDGE_ANY
} gpio_edge_t;

typedef enum
{
    GPIO_INPUT_HIGH_Z,
    GPIO_INPUT_PULLUP,
    GPIO_OUTPUT_ACTIVE_HIGH,
    GPIO_OUTPUT_ACTIVE_LOW,
} gpio_data_direction_t;

typedef enum
{
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    NBR_OF_PORTS
} gpio_port_t;

typedef enum {
    GPIO_INT_0,
    GPIO_INT_1,
    NBR_OF_EXTINT
} gpio_int_t;

typedef struct {
    gpio_port_t port;
    uint8_t pin;
    gpio_data_direction_t direction;
} gpio_t;

typedef void (*gpio_extint_cbk_t)(volatile void*);
typedef void (*gpio_pcint_cbk_t)(uint8_t mask, volatile void*);

int gpio_init(gpio_t *self, gpio_port_t port, uint8_t pin, gpio_data_direction_t direction);
int gpio_set(gpio_t *self);
int gpio_reset(gpio_t *self);
int gpio_get(gpio_t *self, bool *state);
int gpio_enable_extint(gpio_int_t pin, gpio_edge_t edge, gpio_extint_cbk_t cbk, volatile void *data);
int gpio_enable_pcint(gpio_port_t port, uint8_t mask, gpio_pcint_cbk_t cbk, volatile void *data);

#endif /* __GPIO_H__ */
