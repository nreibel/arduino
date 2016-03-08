#include "lcd.h"
#include "lcd_cfg.h"
#include "lcd_prv.h"
#include "port.h"
#include "bits.h"
#include "os.h"

Std_ReturnType LCD_Init()
{
	Std_ReturnType retval = Status_Not_OK;

	uint8_t port_ddr = 0x0;

	if ( Port_GetDataDirection(Port_Data, &port_ddr) == Status_OK )
	{
		SET_MASK(port_ddr, 0xF);
		Port_SetDataDirection(Port_Data, port_ddr);

		Port_SetPinDataDirection(pin_EN, Output);
		Port_SetPinDataDirection(pin_RS, Output);

		retval = Status_OK;
	}

	/* Give time for hardware to init*/
	Os_Sleep(15);

	/* First write sets data width*/
	Write_Half_Byte(0x2, Low);
	Os_Sleep(4);

	LCD_FunctionSet(DataWidth_4_Bits, CharacterFont_5x8, DisplayLines_2_Line);
	LCD_DisplayControl(TRUE, TRUE, TRUE);
	LCD_ClearDisplay();
	LCD_SetEntryMode(LeftToRight, FALSE);
	LCD_ReturnHome();

	return retval;
}

Std_ReturnType LCD_PrintString(char* chr)
{
	Std_ReturnType retval = Status_OK;

	while(*chr != 0 && retval == Status_OK)
	{
		retval = LCD_Print(*chr);
		chr++;
	}

	return retval;
}

Std_ReturnType LCD_Print(char chr)
{
	Std_ReturnType retval = Status_OK;

	if (chr < 0x20 || chr > 0x7F)
	{
		retval = Status_Not_OK;
	}

	if (retval == Status_OK)
	{
		retval = Write_Byte(chr, High);
	}

	return retval;
}

Std_ReturnType LCD_ClearDisplay()
{
	Std_ReturnType retval = Status_OK;

	uint8_t data = 0x01;

	if (retval == Status_OK)
	{
		retval = Write_Byte(data, Low);
	}

	return retval;
}

Std_ReturnType LCD_ReturnHome()
{
	Std_ReturnType retval = Status_OK;

	uint8_t data = 0x02;

	if (retval == Status_OK)
	{
		retval = Write_Byte(data, Low);
	}

	return retval;
}

Std_ReturnType LCD_SetEntryMode(CursorMoveDirection direction, boolean automaticDisplayShift)
{
	Std_ReturnType retval = Status_OK;

	uint8_t data = 0x40;

	switch(direction)
	{
	case RightToLeft: RESET_BIT(data, 1); break;
	case LeftToRight:   SET_BIT(data, 1); break;
	default: retval = Status_Not_OK; /* Invalid data */
	}

	if (automaticDisplayShift == TRUE) SET_BIT(data, 0);

	if (retval == Status_OK)
	{
		retval = Write_Byte(data, Low);
	}

	return retval;
}

Std_ReturnType LCD_DisplayControl(boolean displayOn, boolean cursonOn, boolean cursorBlinking)
{
	Std_ReturnType retval = Status_OK;

	uint8_t data = 0x08;

	if (displayOn      == TRUE) SET_BIT(data, 2);
	if (cursonOn       == TRUE) SET_BIT(data, 1);
	if (cursorBlinking == TRUE) SET_BIT(data, 0);

	if (retval == Status_OK)
	{
		retval = Write_Byte(data, Low);
	}

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

Std_ReturnType LCD_FunctionSet(InterfaceDataWidth dataWidth, CharacterFont characterFont, DisplayLines displayLines)
{
	Std_ReturnType retval = Status_OK;

	uint8_t data = 0x20;

	switch(dataWidth)
	{
	case DataWidth_4_Bits: RESET_BIT(data, 4); break;
	case DataWidth_8_Bits:   SET_BIT(data, 4); break;
	default: retval = Status_Not_OK; /* Invalid data */
	}

	switch(displayLines)
	{
	case DisplayLines_1_Line: RESET_BIT(data, 3); break;
	case DisplayLines_2_Line:   SET_BIT(data, 3); break;
	default: retval = Status_Not_OK; /* Invalid data */
	}

	switch(characterFont)
	{
	case CharacterFont_5x8: RESET_BIT(data, 2); break;
	case CharacterFont_5x10:  SET_BIT(data, 2); break;
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

	/* Set RS pin */
	Port_SetPinState(pin_RS, rs);

	/* Send trigger for 2ms */
	Port_SetPinState(pin_EN, High);
	Os_Sleep(2);
	Port_SetPinState(pin_EN, Low);

	return retval;
}
