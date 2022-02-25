#ifndef __GPIO_H__
#define __GPIO_H__

#include "types.h"
#include "os_cfg.h"
#include "port_ll.h"

typedef enum {
    GPIO_OK,
    GPIO_INVALID_PORT,
    GPIO_INVALID_PIN,
    GPIO_INVALID_INT,
    GPIO_INVALID_MODE
} gpio_errno_t;

typedef enum
{
    GPIO_EDGE_LOW,
    GPIO_EDGE_ANY,
    GPIO_EDGE_FALLING,
    GPIO_EDGE_RISING
} gpio_edge_t;

typedef enum
{
    GPIO_DIRECTION_NONE,
    GPIO_INPUT_HIGH_Z,
    GPIO_INPUT_PULLUP,
    GPIO_OUTPUT_ACTIVE_HIGH,
    GPIO_OUTPUT_ACTIVE_LOW,
} gpio_data_direction_t;

typedef struct gpio_prv_s {
    port_t port;
    uint8_t pin;
    gpio_data_direction_t direction;
} * gpio_t;

typedef void (*gpio_extint_cbk_t)(extint_t pin, volatile void*);
typedef void (*gpio_pcint_cbk_t)(pcint_t port, uint8_t mask, volatile void*);

#if OS_MALLOC
gpio_t gpio_create(port_t port, uint8_t pin, gpio_data_direction_t direction);
void gpio_destroy(gpio_t self);
#endif // OS_MALLOC

int gpio_init(gpio_t self, port_t port, uint8_t pin, gpio_data_direction_t direction);
int gpio_set_data_direction(gpio_t self, gpio_data_direction_t direction);
int gpio_set(gpio_t self);
int gpio_reset(gpio_t self);
int gpio_toggle(gpio_t self);
int gpio_get(gpio_t self, bool *state);

int gpio_enable_extint(extint_t pin, gpio_edge_t edge, gpio_extint_cbk_t cbk, volatile void *data);
int gpio_disable_extint(extint_t pin);

int gpio_enable_pcint(pcint_t port, uint8_t mask, gpio_pcint_cbk_t cbk, volatile void *data);
int gpio_disable_pcint(pcint_t port);

#endif // __GPIO_H__
