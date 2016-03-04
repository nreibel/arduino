#include "lcd.h"
#include "lcd_cfg.h"
#include "port.h"
#include "bits.h"
#include "os.h"

Std_ReturnType Write_Byte(uint8_t data, PinState rs);
Std_ReturnType Write_Half_Byte(uint8_t data, PinState rs);

Std_ReturnType LCD_Init()
{
	Std_ReturnType retval = Status_Not_OK;

	uint8_t port_ddr = 0x0;

	if ( Port_GetDataDirection(Port_Data, &port_ddr) == Status_OK )
	{
		SET_MASK(port_ddr, 0xF);
		Port_SetDataDirection(Port_Data, port_ddr);

		Port_SetPinDataDirection(Port_EN, Pin_EN, Output);
		Port_SetPinDataDirection(Port_RS, Pin_RS, Output);

		retval = Status_OK;
	}

	// Function set
	Write_Half_Byte(0x2, Low);

	LCD_FunctionSet(DataLength_4bits, DisplayType_5x8, 2);

	// Turn display ON, set cursor blinking
	Write_Byte(0x0F, Low);

	// Set the entry mode
	Write_Byte(0x06, Low);

	// Change the cursor position to the 2nd line, 4th position from the left
	LCD_SetCursor(1, 2);

	// Enter a character "H" into the display
	Write_Byte(0x48, High);

	// Enter a character "e" into the display
	Write_Byte(0x65, High);

	return retval;
}


Std_ReturnType LCD_SetCursor(uint8_t line, uint8_t pos)
{
	Std_ReturnType retval = Status_OK;

	uint8_t data = 0x80;

	switch(line)
	{
	case 1: RESET_BIT(data, 6); break;
	case 2:   SET_BIT(data, 6); break;
	default: retval = Status_Not_OK; /* Invalid data */
	}

	if (pos <= 0xF)
	{
		SET_MASK(data, pos);
	}
	else
	{
		retval = Status_Not_OK;
	}

	if (retval == Status_OK)
	{
		retval = Write_Byte(data, Low);
	}

	return retval;
}

Std_ReturnType LCD_FunctionSet(DataLength dl, DisplayType ft, uint8_t nbrOfLines)
{
	Std_ReturnType retval = Status_OK;

	uint8_t data = 0x20;

	switch(dl)
	{
	case DataLength_4bits: RESET_BIT(data, 4); break;
	case DataLength_8bits:   SET_BIT(data, 4); break;
	default: retval = Status_Not_OK; /* Invalid data */
	}

	switch(nbrOfLines)
	{
	case 1: RESET_BIT(data, 3); break;
	case 2:   SET_BIT(data, 3); break;
	default: retval = Status_Not_OK; /* Invalid data */
	}

	switch(ft)
	{
	case DisplayType_5x8: RESET_BIT(data, 2); break;
	case DisplayType_5x10:  SET_BIT(data, 2); break;
	default: retval = Status_Not_OK; /* Invalid data */
	}

	if (retval == Status_OK)
	{
		retval = Write_Byte(data, Low);
	}

	return retval;
}

Std_ReturnType Write_Byte(uint8_t data, PinState rs)
{
	Std_ReturnType retval = Status_Not_OK;
	Std_ReturnType retval_high_nibble = Status_Not_OK;
	Std_ReturnType retval_low_nibble = Status_Not_OK;

	// Write high nibble
	retval_high_nibble = Write_Half_Byte( HIGH_NIBBLE(data), rs );

	// Write low nibble
	retval_low_nibble = Write_Half_Byte( LOW_NIBBLE(data), rs );

	if (retval_high_nibble == Status_OK && retval_low_nibble == Status_OK)
	{
		retval = Status_OK;
	}

	return retval;
}

Std_ReturnType Write_Half_Byte(uint8_t data, PinState rs)
{
	Std_ReturnType retval = Status_Not_OK;

	uint8_t portVal;

	if (Status_OK == Port_GetValue(Port_Data, &portVal))
	{
		RESET_MASK(portVal, 0xF);
		SET_MASK(portVal, data & 0xF);
		Port_SetValue(Port_Data, portVal);

		retval = Status_OK;
	}

	Port_SetPinState(Port_RS, Pin_RS, rs);

	// Send trigger for
	Port_SetPinState(Port_EN, Pin_EN, High);
	Os_Sleep(2);
	Port_SetPinState(Port_EN, Pin_EN, Low);

	return retval;
}
