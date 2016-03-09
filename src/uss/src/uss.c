#include "uss.h"
#include "uss_prv.h"
#include "os.h"
#include "types.h"
#include "port.h"
#include "bits.h"
#include "port_cfg.h"
#include "avr/interrupt.h"
#include "avr/io.h"

volatile static Uss_InternalState ussStateMachine  = Uninitialized;

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
		RESET_BIT(EIMSK, 1); // Disable INT1 interrupt
		ussStateMachine = MeasurementAvailable;
		break;
	default:
		break;
	}
}

void USS_Init()
{
	Port_SetPinDataDirection(pinTrigger, Output);
	Port_SetPinDataDirection(pinEcho, Input);
	ussStateMachine = Ready;
	Os_EnableInterrupts();
}

Std_ReturnType USS_GetDistance(uint16_t *distance)
{
	Std_ReturnType retval = Status_Not_OK;

	switch(ussStateMachine)
	{
	case MeasurementAvailable:
		*distance = TICKS_TO_CM(TCNT1);
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
		// Init timer 1
		TCCR1A = 0x0; // Normal mode
		TCCR1B = 0x2; // Set prescaler to 8 = 2 ticks/us

		// Start sending trigger
		Port_SetPinState(pinTrigger, High);

		// Wait 15us
		TCNT1 = 0;
		while(TCNT1 < 30);

		// Stop sending trigger
		Port_SetPinState(pinTrigger, Low);

		// Enable interrupt on rising edge of INT1
		EICRA = 0xC;
		SET_BIT(EIMSK, INT1);

		ussStateMachine = WaitForEcho;
		retval = Status_OK;
	}

	return retval;
}
