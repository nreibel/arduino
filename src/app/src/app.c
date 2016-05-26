#include "os.h"
#include "app.h"
#include "app_prv.h"
#include "app_cfg.h"
#include "timer.h"
#include "port.h"
#include "serial.h"
#include "bits.h"
#include "eeprom.h"
#include "keys.h"

/*
 * Set debug ON to print to the serial output (9600bps) instead of sending MIDI commands
 */
#define DEBUG OFF

#if DEBUG == ON
#include <stdio.h>
#define BAUD_RATE 9600
#else
#define BAUD_RATE 31250
#endif

/*
 * Global variables
 */
static WhammyMode whammyMode = Whammy_2_Oct_Up;
static boolean    chordsMode = TRUE;
static boolean    bypassMode = TRUE;
static PinDef    *activeLED  = NULL_PTR;

void App_Init()
{
	Port_SetPinDataDirection(Pin_LED_Chords,  Output);
	Port_SetPinDataDirection(Pin_LED_Classic, Output);

	// Init keys handling
	Keys_Init();

	// Init serial for MIDI or debug communication
	Serial_Init(BAUD_RATE);

	// Set default channel at boot
	Whammy_ProgramChange();

	// Set up tasks
	Timer_StartTask(Timer_MainTask, 10, &Task_MainCyclic);
}

// Send MIDI Program Change command through Serial
void Whammy_ProgramChange()
{
	uint8_t newProgram = whammyMode;

	if (bypassMode == TRUE)
	{
		newProgram += WHAMMY_CHANNEL_BYPASS_OFFSET;
	}

	if (chordsMode == TRUE)
	{
		newProgram += WHAMMY_CHORDS_MODE_OFFSET;
		activeLED = &Pin_LED_Chords;
		Port_SetPinState(Pin_LED_Chords,  High);
		Port_SetPinState(Pin_LED_Classic, Low);
	}
	else
	{
		activeLED = &Pin_LED_Classic;
		Port_SetPinState(Pin_LED_Chords,  Low);
		Port_SetPinState(Pin_LED_Classic, High);
	}

#if DEBUG == ON
	static char message[20];
	sprintf(message, "Program %02d\r\n", newProgram);
	Serial_AsyncPrint(message, 20);
#else
	static char midiMessage[2] = {0xC0, 0x0};
	midiMessage[1] = newProgram;
	Serial_AsyncPrint(midiMessage, 2);
#endif
}

typedef enum {
	Idle,
	PresetPressed,
	WaitBeforeNextCycle,
	SavePreset,
	BlinkLed,
	WaitForBlink,
	RestorePreset,
} MainState;

// Main task
void Task_MainCyclic(void)
{
	static MainState state     = Idle;
	static Key selectedPreset  = NUMBER_OF_KEYS;
	static int ledBlinkCounter = 0;

	/*
	 * Don't need to set this as background task.
	 * Keep CPU cycle for more important tasks :)
	 */
	Keys_CyclicTask();

	switch(state)
	{
	case Idle:
	{
		Key key = NUMBER_OF_KEYS;
		if ( Keys_IsKeyPressed() && Keys_GetKeyPressed(&key) == Status_OK )
		{
			switch(key)
			{
			case ScrollUp:

				bypassMode = TRUE;
				INCREMENT_MOD(whammyMode, WHAMMY_MAX_MODE);

				// We looped back to the first mode
				if (whammyMode == Whammy_2_Oct_Up)
				{
					chordsMode = NOT(chordsMode);
				}

				Whammy_ProgramChange();

				state = WaitBeforeNextCycle;
				break;

			case ScrollDown:

				bypassMode = TRUE;
				DECREMENT_MOD(whammyMode, WHAMMY_MAX_MODE);

				// We looped back to the last mode
				if (whammyMode == Harmony_Oct_Up_Oct_Down)
				{
					chordsMode = NOT(chordsMode);
				}

				Whammy_ProgramChange();

				state = WaitBeforeNextCycle;
				break;

			default:
				// Other : preset key
				Timer_StartTimer(Timer_ButtonPress, SAVE_PRESET_DELAY);
				selectedPreset = key;
				state = PresetPressed;
				break;
			}
		}
		break;
	}

	case PresetPressed:
	{
		boolean timerElapsed = FALSE;

		// Button timer has elapsed : save preset
		if ( Timer_IsElapsed(Timer_ButtonPress, &timerElapsed) == Status_OK && timerElapsed == TRUE )
		{
			Timer_Disable(Timer_ButtonPress);
			state = SavePreset;
		}
		// Button released before timer elapsed : restore preset
		else if ( !Keys_IsKeyPressed() )
		{
			Timer_Disable(Timer_ButtonPress);
			state = RestorePreset;
		}

		break;
	}

	case RestorePreset:
	{
		if (selectedPreset >= Preset1 && selectedPreset <= Preset4)
		{
			EEPROM_Data data;
			int readAddress = selectedPreset * sizeof(EEPROM_Data);
			EEPROM_SyncRead(readAddress, (uint8_t*) &data, sizeof(data));

			if (data.chordsMode == chordsMode && data.whammyMode == whammyMode)
			{
				// Toggle active preset
				bypassMode = NOT(bypassMode);
			}
			else
			{
				// Restore saved preset
				bypassMode = FALSE;
				whammyMode = data.whammyMode;
				chordsMode = data.chordsMode;
			}

			Whammy_ProgramChange();
		}

		state = WaitBeforeNextCycle;
		break;
	}

	case SavePreset:
	{
		if (selectedPreset >= Preset1 && selectedPreset <= Preset4)
		{
			int writeAddress = selectedPreset * sizeof(EEPROM_Data);

			static EEPROM_Data data;
			data.chordsMode = chordsMode;
			data.whammyMode = whammyMode;

			EEPROM_AsyncWrite(writeAddress, (uint8_t*) &data, sizeof(EEPROM_Data));

			Timer_StartTimer(Timer_LedBlink, LED_BLINK_DELAY);
			ledBlinkCounter = 0;
			state = BlinkLed;
		}
		else
		{
			state = WaitBeforeNextCycle;
		}

		break;
	}

	case WaitForBlink:
	{
		boolean timerElapsed = FALSE;
		if (Timer_IsElapsed(Timer_LedBlink, &timerElapsed) == Status_OK && timerElapsed == TRUE)
		{
			state = BlinkLed;
		}

		break;
	}

	case BlinkLed:
	{
		PinState ledStatus = Low;

		// Invert pin state
		Port_GetPinState(*activeLED, &ledStatus);
		ledStatus = (ledStatus == Low ? High : Low);
		Port_SetPinState(*activeLED, ledStatus);

		if (ledBlinkCounter++ > 2*NUMBER_OF_BLINKS)
		{
			Timer_Disable(Timer_LedBlink);
			state = WaitBeforeNextCycle;
		}
		else
		{
			state = WaitForBlink;
		}

		break;
	}

	case WaitBeforeNextCycle:
	{
		// Wait for key release, eeprom and serial ready before next cycle
		if ( !Keys_IsKeyPressed() && Serial_IsReady() && EEPROM_IsReady() )
		{
			state = Idle;
		}

		break;
	}

	}
}
