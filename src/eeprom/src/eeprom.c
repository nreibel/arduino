#include "eeprom.h"
#include "eeprom_prv.h"
#include "bits.h"
#include "avr/io.h"

static EepromState       eepromState = EEPROM_Ready;
static EepromDataBuffer writeBuffer;
static EepromDataBuffer readBuffer;

boolean EEPROM_IsReady(void)
{
	boolean retval = FALSE;

	if (eepromState == EEPROM_Ready)
	{
		retval = TRUE;
	}

	return retval;
}

void EEPROM_BackgroundTask()
{
	if (eepromState == EEPROM_Writing && !IS_SET_BIT(EECR, EEPE))
	{
		EEAR = writeBuffer.romAddr++;
		EEDR = *(writeBuffer.ramAddr++);
		writeBuffer.length--;

		/* Set Master Program Enable first, then Program Enable */
		SET_BIT(EECR, EEMPE);
		SET_BIT(EECR, EEPE);

		if (writeBuffer.length == 0)
		{
			eepromState = EEPROM_Ready;
		}
	}
	else if (eepromState == EEPROM_Reading && !IS_SET_BIT(EECR, EEPE))
	{
		EEAR = readBuffer.romAddr++;
		SET_BIT(EECR, EERE);
		*(readBuffer.ramAddr++) = EEDR;
		readBuffer.length--;

		if (readBuffer.length == 0)
		{
			eepromState = EEPROM_Ready;
		}
	}
}

Std_ReturnType EEPROM_AsyncWrite(int ucAddress, uint8_t *ucData, int dataLength)
{
	Std_ReturnType retval = Status_Not_OK;

	if (eepromState == EEPROM_Ready && dataLength > 0)
	{
		/* Set Programming mode */
		RESET_BIT(EECR, EEPM0);
		RESET_BIT(EECR, EEPM1);

		writeBuffer.romAddr = ucAddress;
		writeBuffer.ramAddr = ucData;
		writeBuffer.length  = dataLength;

		eepromState = EEPROM_Writing;

		retval = Status_Pending;
	}

	return retval;
}

Std_ReturnType EEPROM_AsyncRead(int ucAddress, uint8_t *ucData, int dataLength)
{
	Std_ReturnType retval = Status_Not_OK;

	if (eepromState == EEPROM_Ready && dataLength > 0)
	{
		readBuffer.romAddr = ucAddress;
		readBuffer.ramAddr = ucData;
		readBuffer.length  = dataLength;

		eepromState = EEPROM_Reading;

		retval = Status_Pending;
	}

	return retval;
}

Std_ReturnType EEPROM_SyncWrite(int ucAddress, uint8_t* ucData, int length)
{
	Std_ReturnType retval = Status_Not_OK;

	if (eepromState == EEPROM_Ready)
	{
		/* Wait for completion of previous write */
		while (IS_SET_BIT(EECR, EEPE));

		/* Set Programming mode : erase and write in atomic operation */
		RESET_BIT(EECR, EEPM0);
		RESET_BIT(EECR, EEPM1);

		while (length-- > 0)
		{
			EEAR = ucAddress++;
			EEDR = *(ucData++);

			/* Set Master Program Enable first, then Program Enable */
			SET_BIT(EECR, EEMPE);
			SET_BIT(EECR, EEPE);

			while (IS_SET_BIT(EECR, EEPE));
		}

		retval = Status_OK;
	}

	return retval;
}

Std_ReturnType EEPROM_SyncRead(int ucAddress, uint8_t* ucData, int length)
{
	Std_ReturnType retval = Status_Not_OK;

	if (eepromState == EEPROM_Ready)
	{
		/* Wait for completion of previous write */
		while (IS_SET_BIT(EECR, EEPE));

		while(length-- > 0)
		{
			EEAR = ucAddress++;
			SET_BIT(EECR, EERE);
			*(ucData++) = EEDR;
		}

		retval = Status_OK;
	}

	return retval;
}

