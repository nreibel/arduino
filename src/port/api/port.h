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

Std_ReturnType Port_SetDataDirection    (Port port, uint8_t direction);
Std_ReturnType Port_GetDataDirection    (Port port, uint8_t* direction);
Std_ReturnType Port_SetValue            (Port port, uint8_t portValue);
Std_ReturnType Port_GetValue            (Port port, uint8_t* portValue);

Std_ReturnType Port_SetPinDataDirection (PinDef pinDef, DataDirection direction);
Std_ReturnType Port_SetPinState         (PinDef pinDef, PinState pinState);
Std_ReturnType Port_GetPinState         (PinDef pinDef, PinState* pinState);

#endif /* SRC_PORT_API_PORT_H_ */
