#include "timer.h"
#include "timer_ll.h"

/*
 * Private data
 */

static struct {
    bool init;
    uint8_t prescaler;
} instances[NUMBER_OF_TIMERS];

extern const uint8_t __timer_prescaler_table[NUMBER_OF_TIMERS][NUMBER_OF_PRESCALERS];

/*
 * Public methods
 */

int timer_init(timer_t timer, timer_config_t * config)
{
    if (timer >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    switch(config->prescaler)
    {
        case TIMER_PRESCALER_STOPPED:
        case TIMER_PRESCALER_1:
        case TIMER_PRESCALER_8:
        case TIMER_PRESCALER_32:
        case TIMER_PRESCALER_64:
        case TIMER_PRESCALER_128:
        case TIMER_PRESCALER_256:
        case TIMER_PRESCALER_1024:

            // Prescaler not supported by this timer
            if (__timer_prescaler_table[timer][config->prescaler] == 0xFF)
                return -TIMER_ERROR_PRESCALER;

            break;

        default:
            return -TIMER_ERROR_PRESCALER;
    }

    // Reset device
    timer_ll_reset(timer);

    // Enable device
    timer_ll_power_enable(timer);

    // Set Waveform Generation Mode
    switch(config->mode)
    {
        case TIMER_MODE_NORMAL:
            timer_ll_set_wgm(timer, TIMER_LL_WGM_NORMAL);
            break;

        case TIMER_MODE_CTC:
            timer_ll_set_wgm(timer, TIMER_LL_WGM_CTC);
            break;

        case TIMER_MODE_FAST_PWM:
            timer_ll_set_wgm(timer, TIMER_LL_WGM_FAST_PWM);
            break;

        case TIMER_MODE_PHASE_CORRECT_PWM:
            timer_ll_set_wgm(timer, TIMER_LL_WGM_PHASE_CORRECT_PWM);
            break;

        default:
            return -TIMER_ERROR_MODE;
    }

    // Set Output Compare Mode
    switch(config->oca_mode)
    {
        case TIMER_OCA_MODE_TOP:
            timer_ll_set_oca_mode(timer, true);
            break;

        case TIMER_OCA_MODE_PWM:
            timer_ll_set_oca_mode(timer, false);
            break;

        default:
            return -TIMER_ERROR_OCA_MODE;
    }

    // Set Compare Outpout Mode A
    switch(config->output_mode_a)
    {
        case TIMER_OUTPUT_MODE_DISABLED:
            timer_ll_set_coma(timer, TIMER_LL_COM_DISABLED);
            break;

        case TIMER_OUTPUT_MODE_DEFAULT:
            timer_ll_set_coma(timer, TIMER_LL_COM_DEFAULT);
            break;

        case TIMER_OUTPUT_MODE_INVERTED:
            timer_ll_set_coma(timer, TIMER_LL_COM_INVERTED);
            break;

        case TIMER_OUTPUT_MODE_TOGGLE:
            timer_ll_set_coma(timer, TIMER_LL_COM_TOGGLE);
            break;

        default:
            return -TIMER_ERROR_OUTPUT_MODE;
    }

    // Set Compare Outpout Mode B
    switch(config->output_mode_b)
    {
        case TIMER_OUTPUT_MODE_DISABLED:
            timer_ll_set_comb(timer, TIMER_LL_COM_DISABLED);
            break;

        case TIMER_OUTPUT_MODE_DEFAULT:
            timer_ll_set_comb(timer, TIMER_LL_COM_DEFAULT);
            break;

        case TIMER_OUTPUT_MODE_INVERTED:
            timer_ll_set_comb(timer, TIMER_LL_COM_INVERTED);
            break;

        case TIMER_OUTPUT_MODE_TOGGLE:
            timer_ll_set_comb(timer, TIMER_LL_COM_TOGGLE);
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
            timer_ll_enable_oca(timer);
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
            timer_ll_enable_ocb(timer);
            break;

        case TIMER_OUTPUT_MODE_DISABLED:
            // Do nothing
            break;

        default:
            return -TIMER_ERROR_OUTPUT_MODE;
    }

    // Set Output Compare Register A/B
    timer_ll_set_ocra(timer, config->ocra);
    timer_ll_set_ocrb(timer, config->ocrb);

    // Enable interrupts
    uint8_t imask = 0;

    if (config->overflow_interrupt)
        imask |= TIMER_LL_INT_OVF;

    if (config->ocma_interrupt)
        imask |= TIMER_LL_INT_OCM_A;

    if (config->ocmb_interrupt)
        imask |= TIMER_LL_INT_OCM_B;

    timer_ll_set_imask(timer, imask);

    instances[timer].prescaler = __timer_prescaler_table[timer][config->prescaler];
    instances[timer].init = true;

    return TIMER_OK;
}

int timer_start(timer_t timer)
{
    if (timer >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    if (!instances[timer].init)
        return -TIMER_ERROR_INIT;

    if (timer_ll_set_prescaler(timer, instances[timer].prescaler) != TIMER_LL_OK)
        return -TIMER_ERROR_PRESCALER;

    return TIMER_OK;
}

int timer_stop(timer_t timer)
{
    if (timer >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    if (!instances[timer].init)
        return -TIMER_ERROR_INIT;

    if (timer_ll_set_prescaler(timer, TIMER_PRESCALER_STOPPED) != TIMER_LL_OK)
        return -TIMER_ERROR_PRESCALER;

    return TIMER_OK;
}
