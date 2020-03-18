#ifndef SRC_PORT_API_PORT_H_
#define SRC_PORT_API_PORT_H_

#include "types.h"
#include "port_cfg.h"

typedef struct {
    Port port;
    Pin pin;
} PinDef;

typedef enum
{
    Input,
    Output
} DataDirection;

typedef enum
{
    Low,
    High
} PinState;

typedef enum
{
    Edge_LowLevel = 0,
    Edge_Both     = 1,
    Edge_Falling  = 2,
    Edge_Rising   = 3,
} Edge;

typedef enum
{
    ExtInt_INT0 = 0,
    ExtInt_INT1 = 1,
} ExtInt;

Std_ReturnType Port_EnableInt           (ExtInt input, Edge edge, Callback cbk, void *data);

Std_ReturnType Port_SetDataDirection    (Port port, uint8_t direction);
Std_ReturnType Port_GetDataDirection    (Port port, uint8_t* direction);
Std_ReturnType Port_SetValue            (Port port, uint8_t portValue);
Std_ReturnType Port_GetValue            (Port port, uint8_t* portValue);

Std_ReturnType Port_SetPinDataDirection (PinDef pinDef, DataDirection direction);
Std_ReturnType Port_SetPinState         (PinDef pinDef, PinState pinState);
Std_ReturnType Port_GetPinState         (PinDef pinDef, PinState* pinState);

#endif /* SRC_PORT_API_PORT_H_ */
