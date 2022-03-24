#ifndef GPIO_LL_API_H__
#define GPIO_LL_API_H__

#include "types.h"
#include "port.h"

enum {
    GPIO_LL_EDGE_LOW     = 0x0,
    GPIO_LL_EDGE_ANY     = 0x1,
    GPIO_LL_EDGE_FALLING = 0x2,
    GPIO_LL_EDGE_RISING  = 0x3
};

typedef struct {
    uint8_t pin;
    uint8_t ddr;
    uint8_t port;
} volatile * mem_port_t;

// External interrupts callbacks
extern void gpio_extint_cbk(extint_t i);
extern void gpio_pcint_cbk(pcint_t i, uint8_t val);

// Port operation
void gpio_ll_set_port_data_direction(port_t port, uint8_t mask);
uint8_t gpio_ll_get_port_value(port_t port);
void gpio_ll_set_port_value(port_t port, uint8_t mask);
void gpio_ll_toggle_port(port_t port, uint8_t mask);

// GPIO
void gpio_ll_set_data_direction(port_t port, uint8_t pin, bool direction);
bool gpio_ll_get(port_t port, uint8_t pin);
void gpio_ll_set(port_t port, uint8_t pin);
void gpio_ll_toggle(port_t port, uint8_t pin);
void gpio_ll_reset(port_t port, uint8_t pin);

// External Interrupts
void gpio_ll_set_edge(extint_t pin, uint8_t edge);
void gpio_ll_enable_extint(extint_t pin);
void gpio_ll_disable_extint(extint_t pin);

// Pin Change Interrupts
void gpio_ll_enable_pcint(pcint_t port, uint8_t mask);
void gpio_ll_disable_pcint(pcint_t port);

#endif // GPIO_LL_API_H__
