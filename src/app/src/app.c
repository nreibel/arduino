#include "os.h"
#include "app.h"
#include "app_prv.h"
#include "app_cfg.h"
#include "timer.h"
#include "port.h"
#include "serial.h"
#include "bits.h"
#include "avr/io.h"

static WhammyMode whammyMode  = Whammy_2_Oct_Up;
static boolean    chordsMode  = TRUE;
static boolean    bypassMode  = TRUE;

void App_Init()
{
	Port_SetPinDataDirection(Pin_LED,     Output);
	Port_SetPinDataDirection(Pin_BtnUp,   Input);
	Port_SetPinDataDirection(Pin_BtnDown, Input);
	Port_SetPinDataDirection(Pin_Preset,  Input);

	Serial_Init();

	// Set default channel at boot
	Whammy_ProgramChange(whammyMode, chordsMode, bypassMode);

	// Set up tasks
	Timer_StartTask(Timer_MainTask, 10, &Task_MainCyclic);
}

// Send MIDI Program Change command through Serial
void Whammy_ProgramChange(WhammyMode newProgram, boolean chordsMode, boolean bypass)
{
	if (bypass == TRUE)
	{
		newProgram += WHAMMY_CHANNEL_BYPASS_OFFSET;
	}

	if (chordsMode == TRUE)
	{
		newProgram += WHAMMY_CHORDS_MODE_OFFSET;
		Port_SetPinState(Pin_LED, High);
	}
	else
	{
		Port_SetPinState(Pin_LED, Low);
	}

	static char midiMessage[2] = {0xC0, 0x0};
	midiMessage[1] = newProgram;

	Serial_Print(midiMessage, 2);
}

void EEPROM_write(unsigned int ucAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while ( EECR & (1<<EEPE) );

	/* Set Programming mode */
	EECR = (0<<EEPM1)|(0<<EEPM0);

	/* Set up address and data registers */
	EEAR = ucAddress;
	EEDR = ucData;

	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);

	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}

unsigned char EEPROM_read(unsigned int ucAddress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));

	/* Set up address register */
	EEAR = ucAddress;

	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);

	/* Return data from data register */
	return EEDR;
}


typedef enum {
	Idle,
	ProgramChanged,
	ProgramChangeEnd,
	PresetPressed,
	PresetPressEnd,
	SavePreset,
	BlinkLed,
	RestorePreset,
} MainState;

// Main task
void Task_MainCyclic(void)
{
	static MainState state = Idle;

	PinState upState     = Low;
	PinState downState   = Low;
	PinState presetState = Low;

	Port_GetPinState(Pin_BtnUp,   &upState);
	Port_GetPinState(Pin_BtnDown, &downState);
	Port_GetPinState(Pin_Preset,  &presetState);

	switch(state)
	{
	case Idle:
	{
		// Program UP
		if (upState == High)
		{
			INCREMENT_MOD(whammyMode, WHAMMY_MAX_MODE);

			// We looped back to the first mode
			if (whammyMode == Whammy_2_Oct_Up)
			{
				chordsMode = NOT(chordsMode);
			}

			state = ProgramChanged;
		}
		// Program DOWN
		else if (downState == High)
		{
			DECREMENT_MOD(whammyMode, WHAMMY_MAX_MODE);

			// We looped back to the last mode
			if (whammyMode == Harmony_Oct_Up_Oct_Down)
			{
				chordsMode = NOT(chordsMode);
			}

			state = ProgramChanged;
		}
		// Preset pressed
		else if (presetState == High)
		{
			Timer_StartTimer(Timer_ButtonPress, SAVE_PRESET_DELAY);
			state = PresetPressed;
		}

		break;
	}

	case ProgramChanged:
	{
		bypassMode = TRUE;
		Whammy_ProgramChange(whammyMode, chordsMode, bypassMode);
		state = ProgramChangeEnd;

		break;
	}

	case ProgramChangeEnd:
	{
		// Wait for button released and serial ready before next cycle
		if (upState == Low && downState == Low && Serial_IsReady() == Status_OK)
		{
			state = Idle;
		}

		break;
	}

	case PresetPressed:
	{
		boolean timerElapsed = FALSE;

		// Button timer has elapsed : save preset
		if ( Status_OK == Timer_IsElapsed(Timer_ButtonPress, &timerElapsed) && timerElapsed == TRUE )
		{
			Timer_Disable(Timer_ButtonPress);
			state = SavePreset;
		}
		// Button released before timer elapsed : restore preset
		else if ( presetState == Low )
		{
			Timer_Disable(Timer_ButtonPress);
			state = RestorePreset;
		}

		break;
	}

	case RestorePreset:
	{
		boolean    savedChordsMode  = EEPROM_read(0);
		WhammyMode savedCurrentMode = EEPROM_read(1);

		if (savedChordsMode == chordsMode && savedCurrentMode == whammyMode)
		{
			// Toggle active preset
			bypassMode = NOT(bypassMode);
		}
		else
		{
			// Restore saved preset
			bypassMode  = FALSE;
			whammyMode = savedCurrentMode;
			chordsMode  = savedChordsMode;
		}

		Whammy_ProgramChange(whammyMode, chordsMode, bypassMode);

		state = PresetPressEnd;

		break;
	}

	case SavePreset:
	{
		EEPROM_write(0, chordsMode);
		EEPROM_write(1, whammyMode);

		state = BlinkLed;

		break;
	}

	case BlinkLed:
	{
		int cpt;
		PinState ledStatus    = Low;
		boolean  timerElapsed = FALSE;

		Timer_StartTimer(Timer_LedBlink, LED_BLINK_DELAY);
		Port_GetPinState(Pin_LED, &ledStatus);

		for (cpt = 0 ; cpt < 2*NUMBER_OF_BLINKS ; cpt++)
		{
			// Wait for timer elapsed
			while (Timer_IsElapsed(Timer_LedBlink, &timerElapsed) == Status_OK && timerElapsed != TRUE);

			ledStatus = (ledStatus == Low ? High : Low);
			Port_SetPinState(Pin_LED, ledStatus);
		}

		Timer_Disable(Timer_LedBlink);

		state = PresetPressEnd;

		break;
	}

	case PresetPressEnd:
	{
		// Wait for button release and serial ready before next cycle
		if (presetState == Low && Serial_IsReady() == Status_OK)
		{
			state = Idle;
		}

		break;
	}

	}
}
