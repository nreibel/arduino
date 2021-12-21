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

void pwm_init(pwm_mode_t mode, pwm_prescaler_t cs)
{
    // Enable peripheral
    RESET_BIT(PRR, PRTIM0);

    uint8_t tccr0a;
    uint8_t tccr0b;

    switch(mode)
    {
        case PWM_MODE_PHASE_CORRECT:
            tccr0a = BIT(WGM00);
            break;

        case PWM_MODE_FAST_PWM:
            tccr0a = BIT(WGM01) | BIT(WGM00);
            break;

        default:
            HALT; // TODO
    }

    switch(cs)
    {
        case PWM_PRESCALER_0:
            tccr0b = BIT(CS00);
            break;

        case PWM_PRESCALER_8:
            tccr0b = BIT(CS01);
            break;

        case PWM_PRESCALER_64:
            tccr0b = BIT(CS01) | BIT(CS00);
            break;

        case PWM_PRESCALER_256:
            tccr0b = BIT(CS02);
            break;

        case PWM_PRESCALER_1024:
            tccr0b = BIT(CS02) | BIT(CS00);
            break;

        default:
            HALT; // TODO
    }

    TCCR0A = tccr0a;
    TCCR0B = tccr0b;
}

bool pwm_start(pwm_t self, uint8_t duty_cycle, bool inverted)
{
    uint8_t tccr0a = TCCR0A;
    uint8_t ddrd = DDRD;

    switch(self)
    {
        case PWM_5:
        {
            if (inverted)
            {
                config[self].inverted = TRUE;
                SET_BIT(tccr0a, COM0B0);
                OCR0B = 0xff - duty_cycle;
            }
            else
            {
                config[self].inverted = FALSE;
                RESET_BIT(tccr0a, COM0B0);
                OCR0B = duty_cycle;
            }

            SET_BIT(ddrd, 5);
            SET_BIT(tccr0a, COM0B1);
            break;
        }

        case PWM_6:
        {
            if (inverted)
            {
                config[self].inverted = TRUE;
                SET_BIT(tccr0a, COM0A0);
                OCR0A = 0xff - duty_cycle;
            }
            else
            {
                config[self].inverted = FALSE;
                RESET_BIT(tccr0a, COM0A0);
                OCR0A = duty_cycle;
            }

            SET_BIT(ddrd, 6);
            SET_BIT(tccr0a, COM0A1);
            break;
        }

        default: return FALSE;
    }

    DDRD = ddrd;
    TCCR0A = tccr0a;

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
