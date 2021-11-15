#ifndef __GPIO_H__
#define __GPIO_H__

#include "types.h"

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
    GPIO_EDGE_FALLING,
    GPIO_EDGE_RISING,
    GPIO_EDGE_ANY
} gpio_edge_t;

typedef enum
{
    GPIO_DIRECTION_NONE,
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

typedef struct gpio_prv_s {
    gpio_port_t port;
    uint8_t pin;
    gpio_data_direction_t direction;
} * gpio_t;

typedef void (*gpio_extint_cbk_t)(gpio_int_t pin, volatile void*);
typedef void (*gpio_pcint_cbk_t)(gpio_port_t port, uint8_t mask, volatile void*);

extern const gpio_t A0, A1, A2, A3, A4, A5; // Analog inputs
extern const gpio_t D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13; // Digital inputs
extern const gpio_t RX, TX, CS, MOSI, MISO, SCL, LED, SDA, SCL; // Aliases

int gpio_init(gpio_t self, gpio_port_t port, uint8_t pin);
int gpio_set_data_direction(gpio_t self, gpio_data_direction_t direction);
int gpio_set(gpio_t self);
int gpio_reset(gpio_t self);
int gpio_get(gpio_t self, bool *state);
int gpio_enable_extint(gpio_int_t pin, gpio_edge_t edge, gpio_extint_cbk_t cbk, volatile void *data);
int gpio_enable_pcint(gpio_port_t port, uint8_t mask, gpio_pcint_cbk_t cbk, volatile void *data);

#endif // __GPIO_H__
