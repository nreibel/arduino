#include "pwm.h"
#include "types.h"
#include "port.h"
#include "bits.h"
#include "avr/io.h"

void PWM_Init(PWM pin, bool inverted)
{
    // Enable peripheral
    RESET_BIT(PRR, PRTIM0);

    // Set Fast PWM mode
    byte tccr0a = BIT(WGM01) | BIT(WGM00);

    // Set Port_GPIO as OUTPUT
    switch(pin)
    {
        case PWM_5:
            SET_BIT(DDRD, 5);
            if (inverted) SET_BIT(tccr0a, COM0B0);
            break;
        case PWM_6:
            SET_BIT(DDRD, 6);
            if (inverted) SET_BIT(tccr0a, COM0A0);
            break;
        default:
            HALT;
    }

    // No prescaling
    TCCR0B = BIT(CS00);

    TCCR0A = tccr0a;
}

void PWM_StopPWM(PWM pin)
{
    switch(pin)
    {
        case PWM_5:
            RESET_BIT(TCCR0A, COM0B1);
            break;
        case PWM_6:
            RESET_BIT(TCCR0A, COM0A1);
            break;
        default:
            HALT;
    }
}

void PWM_SetPWM(PWM pin, uint8_t dutyCycle)
{
    switch(pin)
    {
        case PWM_5:
            SET_BIT(TCCR0A, COM0B1);
            OCR0B = dutyCycle;
            break;
        case PWM_6:
            SET_BIT(TCCR0A, COM0A1);
            OCR0A = dutyCycle;
            break;
        default:
            HALT;
    }
}
