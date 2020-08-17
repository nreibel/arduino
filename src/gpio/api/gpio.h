#ifndef SRC_GPIO_API_PORT_H_
#define SRC_GPIO_API_PORT_H_

#include "types.h"

typedef enum {
    // Port B
    GPIO_D8,
    GPIO_D9,
    GPIO_D10,
    GPIO_D11,
    GPIO_D12,
    GPIO_D13,

    // Port C
    GPIO_A0,
    GPIO_A1,
    GPIO_A2,
    GPIO_A3,
    GPIO_A4,
    GPIO_A5,

    // Port D
    GPIO_D0,
    GPIO_D1,
    GPIO_D2,
    GPIO_D3,
    GPIO_D4,
    GPIO_D5,
    GPIO_D6,
    GPIO_D7,

    // Aliases
    GPIO_RX   = GPIO_D0,
    GPIO_TX   = GPIO_D1,
    GPIO_INT0 = GPIO_D2,
    GPIO_INT1 = GPIO_D3,
    GPIO_MOSI = GPIO_D11,
    GPIO_MISO = GPIO_D12,
    GPIO_SCK  = GPIO_D13
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
