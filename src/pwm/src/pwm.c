#include "pwm.h"
#include "pwm_prv.h"
#include "types.h"
#include "port.h"
#include "avr/io.h"

void PWM_Init()
{
    Port_SetPinDataDirection(pinA, Output); // PWM on OCR2A
    Port_SetPinDataDirection(pinB, Output); // PWM on OCR2B
}

void PWM_StopPWM()
{
    TCCR0A = 0;
    TCCR0B = 0;
}

void PWM_SetPWM(uint8_t dutyCycle)
{
    TCCR0B = 0x4;  // Set prescaler to 256 = 245Hz
    TCCR0A = 0xB3; // Set OC0A and OC0B in opposite modes
    OCR0A  = dutyCycle;
    OCR0B  = dutyCycle;
}
