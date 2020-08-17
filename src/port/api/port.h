#ifndef SRC_PORT_API_PORT_H_
#define SRC_PORT_API_PORT_H_

#include "types.h"

#warning "PORT module has been deprecated in favor of GPIO."

typedef enum {
    Port_B,
    Port_C,
    Port_D
} Port;

typedef enum
{
    Pin_0 = 0,
    Pin_1,
    Pin_2,
    Pin_3,
    Pin_4,
    Pin_5,
    Pin_6,
    Pin_7,
} Pin;

typedef struct {
    Port port;
    Pin  pin;
} Port_GPIO;

typedef enum
{
    Input,
    Output
} DataDirection;

typedef enum
{
    Low,
    High
} State;

typedef enum
{
    Edge_LowLevel = 0,
    Edge_Both     = 1,
    Edge_Falling  = 2,
    Edge_Rising   = 3,
} Edge;

typedef enum
{
    ExtInt_0,
    ExtInt_1,
} ExtInt;

Std_ReturnType Port_EnableInt           (ExtInt input, Edge edge, Interrupt cbk, volatile void *data);

Std_ReturnType Port_SetDataDirection    (Port port, uint8_t  direction);
Std_ReturnType Port_GetDataDirection    (Port port, uint8_t* direction);
Std_ReturnType Port_SetValue            (Port port, uint8_t  value);
Std_ReturnType Port_GetValue            (Port port, uint8_t* value);

Std_ReturnType Port_RisingEdge          (Port_GPIO gpio);
Std_ReturnType Port_FallingEdge         (Port_GPIO gpio);

Std_ReturnType Port_SetPinDataDirection (Port_GPIO pinDef, DataDirection  direction);
Std_ReturnType Port_GetPinDataDirection (Port_GPIO pinDef, DataDirection *direction);
Std_ReturnType Port_SetPinState         (Port_GPIO pinDef, State  pinState);
Std_ReturnType Port_GetPinState         (Port_GPIO pinDef, State* pinState);

#endif /* SRC_PORT_API_PORT_H_ */
