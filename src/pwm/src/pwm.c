#include "pwm.h"
#include "types.h"
#include "bits.h"
#include "avr/io.h"

typedef struct {
    bool inverted;
} pwm_config_t;

static pwm_config_t config[NUMBER_OF_PWM_CHANNELS] = {
    { FALSE },
    { FALSE }
};

void pwm_init(pwm_prescaler_t cs)
{
    // Enable peripheral
    RESET_BIT(PRR, PRTIM0);

    // Set Fast PWM mode
    TCCR0A = BIT(WGM01) | BIT(WGM00);

    uint8_t tccr0b = TCCR0B;

    switch(cs)
    {
        case PWM_PRESCALER_0:
            SET_BIT(tccr0b, CS00);
            RESET_BIT(tccr0b, CS01);
            RESET_BIT(tccr0b, CS02);
            break;

        case PWM_PRESCALER_8:
            RESET_BIT(tccr0b, CS00);
            SET_BIT(tccr0b, CS01);
            RESET_BIT(tccr0b, CS02);
            break;

        case PWM_PRESCALER_64:
            SET_BIT(tccr0b, CS00);
            SET_BIT(tccr0b, CS01);
            RESET_BIT(tccr0b, CS02);
            break;

        case PWM_PRESCALER_256:
            RESET_BIT(tccr0b, CS00);
            RESET_BIT(tccr0b, CS01);
            SET_BIT(tccr0b, CS02);
            break;

        case PWM_PRESCALER_1024:
            SET_BIT(tccr0b, CS00);
            RESET_BIT(tccr0b, CS01);
            SET_BIT(tccr0b, CS02);
            break;

        default:
            HALT;
    }

    TCCR0B = tccr0b;
}

bool pwm_start(pwm_t self, uint8_t duty_cycle, bool inverted)
{
    switch(self)
    {
        case PWM_5:
        {
            if (inverted)
            {
                config[self].inverted = TRUE;
                SET_BIT(TCCR0A, COM0B0);
                OCR0B = 0xff - duty_cycle;
            }
            else
            {
                config[self].inverted = FALSE;
                RESET_BIT(TCCR0A, COM0B0);
                OCR0B = duty_cycle;
            }

            SET_BIT(DDRD, 5);
            SET_BIT(TCCR0A, COM0B1);
            break;
        }

        case PWM_6:
        {
            if (inverted)
            {
                config[self].inverted = TRUE;
                SET_BIT(TCCR0A, COM0A0);
                OCR0A = 0xff - duty_cycle;
            }
            else
            {
                config[self].inverted = FALSE;
                RESET_BIT(TCCR0A, COM0A0);
                OCR0A = duty_cycle;
            }

            SET_BIT(DDRD, 6);
            SET_BIT(TCCR0A, COM0A1);
            break;
        }

        default: return FALSE;
    }

    return TRUE;
}

bool pwm_set_duty_cycle(pwm_t self, uint8_t duty_cycle)
{
    uint8_t regval = config[self].inverted ? 0xff - duty_cycle : duty_cycle;

    switch(self)
    {
        case PWM_5: OCR0B = regval; break;
        case PWM_6: OCR0A = regval; break;
        default: return FALSE;
    }

    return TRUE;
}

bool pwm_stop(pwm_t self)
{
    switch(self)
    {
        case PWM_5: RESET_BIT(TCCR0A, COM0B1); break;
        case PWM_6: RESET_BIT(TCCR0A, COM0A1); break;
        default: return FALSE;
    }

    return TRUE;
}
