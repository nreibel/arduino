#include "os.h"
#include "timer.h"
#include "timer_ll.h"
#include "bits.h"

/*
 * External data
 */

extern void timer_ll_power_enable(ll_timer_t timer);
extern void timer_ll_set_imask(ll_timer_t timer, uint8_t imask);
extern int timer_ll_set_prescaler(ll_timer_t timer, uint8_t prescaler);

/*
 * Public methods
 */

int timer_init(timer_t self, timer_config_t * config)
{
    if (self == NULL_PTR)
        return -TIMER_ERROR_INSTANCE;

    // Reset device
    self->instance->TCCRA.reg = 0;
    self->instance->TCCRB.reg = 0;

    // Enable device
    timer_ll_power_enable(self->instance);

    // Set Waveform Generation Mode
    self->instance->TCCRA.bits.WGM = config->mode;
    self->instance->TCCRB.bits.WGM = config->oca_mode;

    // Set Compare Outpout Mode A/B
    self->instance->TCCRA.bits.COMA = config->output_mode_a;
    self->instance->TCCRA.bits.COMB = config->output_mode_b;

    // Set Output Compare Register A/B
    self->instance->OCRA = config->ocra;
    self->instance->OCRB = config->ocrb;

    // Enable interrupts
    timer_ll_set_imask(self->instance, config->imask);

    self->prescaler = config->prescaler;
    self->init = TRUE;

    // TODO
//     // Set DDR A
//     switch(config->output_mode_a)
//     {
//         case TIMER_OUTPUT_MODE_TOGGLE:
//         case TIMER_OUTPUT_MODE_DEFAULT:
//         case TIMER_OUTPUT_MODE_INVERTED:
//         {
//             switch(self)
//             {
//                 case TIMER_0: SET_BIT(ddra, 6); break;
//                 case TIMER_2: SET_BIT(ddra, 3); break;
//                 default: return -TIMER_ERROR_INSTANCE;
//             }
//             break;
//         }
//
//         default:
//             // Do nothing
//             break;
//     }
//
//     // Set DDR B
//     switch(config->output_mode_b)
//     {
//         case TIMER_OUTPUT_MODE_TOGGLE:
//         case TIMER_OUTPUT_MODE_DEFAULT:
//         case TIMER_OUTPUT_MODE_INVERTED:
//         {
//             switch(self)
//             {
//                 case TIMER_0: SET_BIT(ddrb, 5); break;
//                 case TIMER_2: SET_BIT(ddrb, 3); break;
//                 default: return -TIMER_ERROR_INSTANCE;
//             }
//             break;
//         }
//
//         default:
//             // Do nothing
//             break;
//     }

    return TIMER_OK;
}

int timer_start(timer_t self)
{
    if (self == NULL_PTR)
        return -TIMER_ERROR_INSTANCE;

    if (!self->init)
        return -TIMER_ERROR_INIT;

    if ( timer_ll_set_prescaler(self->instance, self->prescaler) < 0 )
        return -TIMER_ERROR_PRESCALER;

    return TIMER_OK;
}

int timer_stop(timer_t self)
{
    if (self == NULL_PTR)
        return -TIMER_ERROR_INSTANCE;

    if (!self->init)
        return -TIMER_ERROR_INIT;

    if ( timer_ll_set_prescaler(self->instance, TIMER_PRESCALER_STOPPED) < 0 )
        return -TIMER_ERROR_PRESCALER;

    return TIMER_OK;
}

int timer_set_ocra(timer_t self, uint8_t val)
{
    if (self == NULL_PTR)
        return -TIMER_ERROR_INSTANCE;

    self->instance->OCRA = val;

    return TIMER_OK;
}

int timer_set_ocrb(timer_t self, uint8_t val)
{
    if (self == NULL_PTR)
        return -TIMER_ERROR_INSTANCE;

    self->instance->OCRB = val;

    return TIMER_OK;
}
