#ifndef GPIO_LL_API_H__
#define GPIO_LL_API_H__

#include "types.h"
#include "port_ll.h"

enum {
    GPIO_LL_EDGE_LOW     = 0x0,
    GPIO_LL_EDGE_ANY     = 0x1,
    GPIO_LL_EDGE_FALLING = 0x2,
    GPIO_LL_EDGE_RISING  = 0x3,
};

enum {
    GPIO_LL_OK,
    GPIO_LL_ERROR_PORT,
    GPIO_LL_ERROR_PIN,
};

typedef struct {
    uint8_t pin;
    uint8_t ddr;
    uint8_t port;
} mem_port_t;

// Interrupt callbacks
void gpio_extint_cbk(extint_t i);
void gpio_pcint_cbk(pcint_t i, uint8_t val);

// GPIO
int gpio_ll_set_data_direction(port_t port, uint8_t pin, bool direction);
int gpio_ll_get(port_t port, uint8_t pin, bool *state);
int gpio_ll_set(port_t port, uint8_t pin);
int gpio_ll_toggle(port_t port, uint8_t pin);
int gpio_ll_reset(port_t port, uint8_t pin);

// External interrupts
int gpio_ll_set_edge(extint_t pin, uint8_t edge);
int gpio_ll_enable_extint(extint_t pin);
int gpio_ll_disable_extint(extint_t pin);
int gpio_ll_enable_pcint(pcint_t port, uint8_t mask);
int gpio_ll_disable_pcint(pcint_t port);

#endif // GPIO_LL_API_H__