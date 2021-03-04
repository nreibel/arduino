#ifndef SRC_GPIO_API_PORT_H_
#define SRC_GPIO_API_PORT_H_

#include "types.h"

// typedef enum
// {
//     GPIO_Edge_Low,
//     GPIO_Edge_Falling,
//     GPIO_Edge_Rising,
//     GPIO_Edge_Any
// } GPIO_Edge;
//
// Std_ReturnType GPIO_RisingEdge(GPIO pin);
// Std_ReturnType GPIO_FallingEdge(GPIO pin);
// Std_ReturnType GPIO_EnableInterrupt(GPIO pin, GPIO_Edge edge, Interrupt cbk, volatile void *data);

/*
 * NEW
 */

typedef enum
{
    GPIO_INPUT,        // High-Z
    GPIO_INPUT_PULLUP, // Internal pullup
    GPIO_OUTPUT
} gpio_data_direction_t;

typedef enum
{
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
} gpio_port_t;

typedef struct {
    gpio_port_t port;
    uint8_t pin;
} gpio_t;

void gpio_init(gpio_t *self, gpio_port_t port, uint8_t pin);
void gpio_set_data_direction(gpio_t *self, gpio_data_direction_t direction);
void gpio_set_state(gpio_t *self, bool state);
void gpio_get_state(gpio_t *self, bool *state);

#endif /* SRC_GPIO_API_PORT_H_ */
