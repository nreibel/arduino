#include "uss.h"

#include "os.h"
#include "types.h"
#include "port.h"
#include "bits.h"
#include "port_cfg.h"
#include "avr/interrupt.h"
#include "avr/io.h"

#define TICKS_TO_US(ticks) ((ticks)/2)
#define TICKS_TO_CM(ticks) (TICKS_TO_US(ticks)/58)

typedef enum {
	Uninitialized,
	Ready,
	WaitForEcho,
	MeasuringEcho,
	MeasurementAvailable,
} Uss_InternalState;

volatile static Uss_InternalState ussStateMachine = Uninitialized;
volatile static uint16_t lastMeasurementTimerTicks;

ISR(INT1_vect)
{
	switch(ussStateMachine)
	{
	case WaitForEcho:
		TCNT1  = 0;  // Reset timer value : start measuring echo
		EICRA = 0x8; // Interrupt on falling edge of INT1
		ussStateMachine = MeasuringEcho;
		break;
	case MeasuringEcho:
		TCCR1B = 0x0;        // Stop timer
		RESET_BIT(EIMSK, 1); // Disable interrupts on pin INT1
		lastMeasurementTimerTicks = TCNT1;
		ussStateMachine = MeasurementAvailable;
		break;
	default:
		break;
	}
}

void USS_Init()
{
	Port_SetPinDataDirection(Port_D, Pin_2, Output);
	Port_SetPinDataDirection(Port_D, Pin_3, Input);
	ussStateMachine = Ready;
	Os_EnableInterrupts();
}

Std_ReturnType USS_GetDistance(uint16_t *distance)
{
	Std_ReturnType retval = Status_Not_OK;

	switch(ussStateMachine)
	{
	case MeasurementAvailable:
		*distance = TICKS_TO_CM(lastMeasurementTimerTicks);
		retval = Status_OK;
		break;
	case WaitForEcho:
	case MeasuringEcho:
		retval = Status_Pending;
		break;
	default:
		break;
	}

	return retval;
}

Std_ReturnType USS_TriggerMeasurement()
{
	Std_ReturnType retval = Status_Not_OK;

	if (ussStateMachine == Ready || ussStateMachine == MeasurementAvailable)
	{
		Port_SetPinState(Port_D, Pin_2, High);

		// Init timer 1
		TCCR1A = 0x0; // Normal port operation
		TCCR1B = 0x2; // Set prescaler to 8 = 2 ticks/us
		TCNT1  = 0;   // Reset timer value

		while(TCNT1 < 30); // Wait 15us

		Port_SetPinState(Port_D, Pin_2, Low);

		EICRA = 0xC;       // Interrupt on rising edge of INT1
		SET_BIT(EIMSK, 1); // Enable interrupt on INT1

		ussStateMachine = WaitForEcho;
		retval = Status_OK;
	}

	return retval;
}
