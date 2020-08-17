#ifndef SRC_GPIO_API_PORT_H_
#define SRC_GPIO_API_PORT_H_

#include "types.h"

typedef enum {
    // Port D
    D0,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,

    // Port B
    D8,
    D9,
    D10,
    D11,
    D12,
    D13,

    // Port C
    A0,
    A1,
    A2,
    A3,
    A4,
    A5
} GPIO;

typedef enum
{
    GPIO_Input,
    GPIO_Output
} GPIO_DataDirection;

typedef enum
{
    GPIO_Low,
    GPIO_High
} GPIO_State;

Std_ReturnType GPIO_SetDataDirection(GPIO pin, GPIO_DataDirection direction);
Std_ReturnType GPIO_Get(GPIO pin, GPIO_State *state);
Std_ReturnType GPIO_Set(GPIO pin, GPIO_State state);
Std_ReturnType GPIO_RisingEdge(GPIO pin);
Std_ReturnType GPIO_FallingEdge(GPIO pin);

#endif /* SRC_GPIO_API_PORT_H_ */
