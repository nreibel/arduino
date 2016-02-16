#ifndef SRC_PORT_API_PORT_H_
#define SRC_PORT_API_PORT_H_

#include "types.h"
#include "port_cfg.h"

typedef enum
{
	Input,
	Output
} DataDirection;

typedef enum
{
	Low,
	High
} PinValue;

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
	NbrOfPins
} Pin;

Std_ReturnType Port_SetDataDirection    (Port port, uint8_t direction);
Std_ReturnType Port_SetValue            (Port port, uint8_t portValue);
Std_ReturnType Port_GetValue            (Port port, uint8_t* portValue);

Std_ReturnType Port_SetPinDataDirection (Port port, Pin pin, DataDirection direction);
Std_ReturnType Port_SetPinValue         (Port port, Pin pin, PinValue pinValue);
Std_ReturnType Port_GetPinValue         (Port port, Pin pin, PinValue* pinValue);

#endif /* SRC_PORT_API_PORT_H_ */
