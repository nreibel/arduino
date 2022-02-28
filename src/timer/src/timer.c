#include "os.h"
#include "timer.h"
#include "timer_ll.h"
#include "gpio_ll.h"
#include "bits.h"

// From LL API
extern timer_handle_t TIMER[NUMBER_OF_TIMERS];

extern int timer_ll_power_enable(timer_t timer);
extern int timer_ll_set_imask(timer_t timer, uint8_t imask);
extern int timer_ll_set_prescaler(timer_t timer, uint8_t prescaler);

/*
 * Public methods
 */

int timer_init(timer_t self, timer_config_t * config)
{
    if (self >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    // Reset device
    TIMER[self].instance->TCCRA.reg = 0;
    TIMER[self].instance->TCCRB.reg = 0;

    // Enable device
    timer_ll_power_enable(self);

    // Set Waveform Generation Mode
    switch(config->mode)
    {
        case TIMER_MODE_NORMAL:
        case TIMER_MODE_CTC:
        case TIMER_MODE_FAST_PWM:
        case TIMER_MODE_PHASE_CORRECT_PWM:
            TIMER[self].instance->TCCRA.bits.WGM = config->mode;
            break;

        default:
            return -TIMER_ERROR_MODE;
    }

    switch(config->oca_mode)
    {
        case TIMER_OCA_MODE_TOP:
        case TIMER_OCA_MODE_PWM:
            TIMER[self].instance->TCCRB.bits.WGM = config->oca_mode;
            break;

        default:
            return -TIMER_ERROR_OCA_MODE;
    }

    // Set Compare Outpout Mode A
    switch(config->output_mode_a)
    {
        case TIMER_OUTPUT_MODE_DISABLED:
        case TIMER_OUTPUT_MODE_DEFAULT:
        case TIMER_OUTPUT_MODE_INVERTED:
        case TIMER_OUTPUT_MODE_TOGGLE:
            TIMER[self].instance->TCCRA.bits.COMA = config->output_mode_a;
            break;

        default:
            return -TIMER_ERROR_OUTPUT_MODE;
    }

    // Set Compare Outpout Mode B
    switch(config->output_mode_b)
    {
        case TIMER_OUTPUT_MODE_DISABLED:
        case TIMER_OUTPUT_MODE_DEFAULT:
        case TIMER_OUTPUT_MODE_INVERTED:
        case TIMER_OUTPUT_MODE_TOGGLE:
            TIMER[self].instance->TCCRA.bits.COMB = config->output_mode_b;
            break;

        default:
            return -TIMER_ERROR_OUTPUT_MODE;
    }

    // Set DDR A
    switch(config->output_mode_a)
    {
        case TIMER_OUTPUT_MODE_TOGGLE:
        case TIMER_OUTPUT_MODE_DEFAULT:
        case TIMER_OUTPUT_MODE_INVERTED:
            gpio_ll_set_data_direction(TIMER[self].oca.port, TIMER[self].oca.pin, TRUE);
            break;

        case TIMER_OUTPUT_MODE_DISABLED:
            // Do nothing
            break;

        default:
            return -TIMER_ERROR_OUTPUT_MODE;
    }

    // Set DDR B
    switch(config->output_mode_b)
    {
        case TIMER_OUTPUT_MODE_TOGGLE:
        case TIMER_OUTPUT_MODE_DEFAULT:
        case TIMER_OUTPUT_MODE_INVERTED:
            gpio_ll_set_data_direction(TIMER[self].ocb.port, TIMER[self].ocb.pin, TRUE);
            break;

        case TIMER_OUTPUT_MODE_DISABLED:
            // Do nothing
            break;

        default:
            return -TIMER_ERROR_OUTPUT_MODE;
    }

    // Set Output Compare Register A/B
    TIMER[self].instance->OCRA = config->ocra;
    TIMER[self].instance->OCRB = config->ocrb;

    // Enable interrupts
    timer_ll_set_imask(self, config->imask);

    TIMER[self].prescaler = config->prescaler;
    TIMER[self].init = TRUE;

    return TIMER_OK;
}

int timer_start(timer_t self)
{
    if (self >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    if (!TIMER[self].init)
        return -TIMER_ERROR_INIT;

    if (timer_ll_set_prescaler(self, TIMER[self].prescaler) != TIMER_LL_OK)
        return -TIMER_ERROR_PRESCALER;

    return TIMER_OK;
}

int timer_stop(timer_t self)
{
    if (self >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    if (!TIMER[self].init)
        return -TIMER_ERROR_INIT;

    if (timer_ll_set_prescaler(self, TIMER_PRESCALER_STOPPED) != TIMER_LL_OK)
        return -TIMER_ERROR_PRESCALER;

    return TIMER_OK;
}

int timer_set_ocra(timer_t self, uint8_t val)
{
    if (self >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    TIMER[self].instance->OCRA = val;

    return TIMER_OK;
}

int timer_set_ocrb(timer_t self, uint8_t val)
{
    if (self >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    TIMER[self].instance->OCRB = val;

    return TIMER_OK;
}
