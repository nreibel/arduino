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
// Std_ReturnType GPIO_EnableInterrupt(GPIO pin, GPIO_Edge edge, Interrupt cbk, volatile void *data);

/*
 * NEW
 */

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
} gpio_port_t;

typedef struct {
    gpio_port_t port;
    uint8_t pin;
    gpio_data_direction_t direction;
} gpio_t;

bool gpio_init(gpio_t *self, gpio_port_t port, uint8_t pin, gpio_data_direction_t direction);
bool gpio_set(gpio_t *self);
bool gpio_reset(gpio_t *self);
bool gpio_get(gpio_t *self, bool *state);

#endif /* SRC_GPIO_API_PORT_H_ */
