#include "timer.h"
#include "bits.h"
#include "avr/io.h"

/*
 * Private types
 */

typedef union {
    struct {
        uint8_t WGM : 2;
        uint8_t reserved : 2;
        uint8_t COMB : 2;
        uint8_t COMA : 2;
    } bits;
    uint8_t byte;
} tccra_t;

typedef union {
    struct {
        uint8_t CS : 3;
        uint8_t WGM : 1;
        uint8_t reserved : 2;
        uint8_t FOCB : 1;
        uint8_t FOCA : 1;
    } bits;
    uint8_t byte;
} tccrb_t;

/*
 * Private data
 */

struct {
    bool is_init;
    timer_prescaler_t prescaler;
} timer_cfg[NUMBER_OF_TIMERS] = {
    [TIMER_0] = { FALSE, TIMER_PRESCALER_STOPPED }
};

/*
 * Public methods
 */

int timer_init(timer_t self, timer_config_t * config)
{
    if (self >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    // Enable peripheral
    RESET_BIT(PRR, PRTIM0);

    tccra_t tccra = { .byte = 0 };
    tccrb_t tccrb = { .byte = 0 };

    // Set Waveform Generation Mode
    switch(config->mode)
    {
        case TIMER_MODE_NORMAL:
        case TIMER_MODE_PHASE_CORRECT_PWM:
        case TIMER_MODE_CTC:
        case TIMER_MODE_FAST_PWM:
            tccra.bits.WGM = config->mode;
            break;

        default:
            return -TIMER_ERROR_MODE;
    }

    // Set Prescaler
    switch(config->prescaler)
    {
        case TIMER_PRESCALER_0:
        case TIMER_PRESCALER_8:
        case TIMER_PRESCALER_64:
        case TIMER_PRESCALER_256:
        case TIMER_PRESCALER_1024:
            tccrb.bits.CS = TIMER_PRESCALER_STOPPED;
            timer_cfg[self].prescaler = config->prescaler;
            break;

        default:
            return -TIMER_ERROR_PRESCALER;
    }

    // Set Compare Outpout Mode
    switch(config->output_mode)
    {
        case TIMER_OUTPUT_MODE_DISABLED:
        case TIMER_OUTPUT_MODE_TOGGLE:
        case TIMER_OUTPUT_MODE_DEFAULT:
        case TIMER_OUTPUT_MODE_INVERTED:
            tccra.bits.COMA = config->output_mode;
            tccra.bits.COMB = config->output_mode;
            break;

        default:
            return -TIMER_ERROR_OUTPUT_MODE;
    }

    // Set data direction
    switch(config->output_mode)
    {
        case TIMER_OUTPUT_MODE_DISABLED:
            // Do nothing
            break;

        case TIMER_OUTPUT_MODE_TOGGLE:
        case TIMER_OUTPUT_MODE_DEFAULT:
        case TIMER_OUTPUT_MODE_INVERTED:
        {
            uint8_t ddrd = DDRD;
            SET_BIT(ddrd, 5);
            SET_BIT(ddrd, 6);
            DDRD = ddrd;
            break;
        }

        default:
            return -TIMER_ERROR_OUTPUT_MODE;
    }

    switch(config->oca_mode)
    {
        case TIMER_OCA_MODE_PWM:
        case TIMER_OCA_MODE_TOP:
            tccrb.bits.WGM = config->oca_mode;
            break;

        default:
            return -TIMER_ERROR_OCA_MODE;
    }

    OCR0A = config->ocra;
    OCR0B = config->ocrb;
    TCCR0A = tccra.byte;
    TCCR0B = tccrb.byte;

    timer_cfg[self].is_init = TRUE;

    return TIMER_OK;
}

int timer_start(timer_t self)
{
    if (self >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    if (!timer_cfg[self].is_init)
        return -TIMER_ERROR_NOT_INIT;

    tccrb_t tccrb = { .byte = TCCR0B };
    tccrb.bits.CS = timer_cfg[self].prescaler;
    TCCR0B = tccrb.byte;

    return TIMER_OK;
}

int timer_stop(timer_t self)
{
    if (self >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    if (!timer_cfg[self].is_init)
        return -TIMER_ERROR_NOT_INIT;

    tccrb_t tccrb = { .byte = TCCR0B };
    tccrb.bits.CS = TIMER_PRESCALER_STOPPED;
    TCCR0B = tccrb.byte;

    return TIMER_OK;
}
