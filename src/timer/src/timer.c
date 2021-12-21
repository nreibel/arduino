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
    [TIMER_0] = { FALSE, TIMER_PRESCALER_STOPPED },
    [TIMER_2] = { FALSE, TIMER_PRESCALER_STOPPED },
};

/*
 * Public methods
 */

int timer_init(timer_t self, timer_config_t * config)
{
    if (self >= NUMBER_OF_TIMERS)
        return -TIMER_ERROR_INSTANCE;

    tccra_t tccra = { .byte = 0 };
    tccrb_t tccrb = { .byte = 0 };
    uint8_t ddr = 0;

    switch(self)
    {
        case TIMER_0:
        {
            ddr = DDRD;

            switch(config->prescaler)
            {
                case TIMER_PRESCALER_0:    timer_cfg[self].prescaler = 0x1; break;
                case TIMER_PRESCALER_8:    timer_cfg[self].prescaler = 0x2; break;
                case TIMER_PRESCALER_64:   timer_cfg[self].prescaler = 0x3; break;
                case TIMER_PRESCALER_256:  timer_cfg[self].prescaler = 0x4; break;
                case TIMER_PRESCALER_1024: timer_cfg[self].prescaler = 0x5; break;
                default: return -TIMER_ERROR_PRESCALER;
            }

            break;
        }

        case TIMER_2:
        {
            // TODO : read DDR

            switch(config->prescaler)
            {
                case TIMER_PRESCALER_0:    timer_cfg[self].prescaler = 0x1; break;
                case TIMER_PRESCALER_8:    timer_cfg[self].prescaler = 0x2; break;
                case TIMER_PRESCALER_32:   timer_cfg[self].prescaler = 0x3; break;
                case TIMER_PRESCALER_64:   timer_cfg[self].prescaler = 0x4; break;
                case TIMER_PRESCALER_128:  timer_cfg[self].prescaler = 0x5; break;
                case TIMER_PRESCALER_256:  timer_cfg[self].prescaler = 0x6; break;
                case TIMER_PRESCALER_1024: timer_cfg[self].prescaler = 0x7; break;
                default: return -TIMER_ERROR_PRESCALER;
            }

            break;
        }

        default:
            return -TIMER_ERROR_INSTANCE;
    }

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
            switch(self)
            {
                case TIMER_0:
                {
                    SET_BIT(ddr, 5);
                    SET_BIT(ddr, 6);
                    break;
                }

                case TIMER_2:
                {
                    // TODO
                    break;
                }

                default:
                    return -TIMER_ERROR_INSTANCE;
            }
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

    switch(self)
    {
        case TIMER_0:
            RESET_BIT(PRR, PRTIM0);
            OCR0A = config->ocra;
            OCR0B = config->ocrb;
            TIMSK0 = config->interrupts & 0x3;
            TCCR0A = tccra.byte;
            TCCR0B = tccrb.byte;
            DDRD = ddr;
            break;

        case TIMER_2:
            RESET_BIT(PRR, PRTIM2);
            OCR2A = config->ocra;
            OCR2B = config->ocrb;
            TIMSK2 = config->interrupts & 0x3;
            TCCR2A = tccra.byte;
            TCCR2B = tccrb.byte;
            // TODO : write DDR
            break;

        default:
            return -TIMER_ERROR_INSTANCE;
    }

    timer_cfg[self].is_init = TRUE;

    return TIMER_OK;
}

int timer_start(timer_t self)
{
    if (self < NUMBER_OF_TIMERS && !timer_cfg[self].is_init)
        return -TIMER_ERROR_NOT_INIT;

    tccrb_t tccrb = { .byte = 0 };

    switch(self)
    {
        case TIMER_0:
            tccrb.byte = TCCR0B;
            tccrb.bits.CS = timer_cfg[self].prescaler;
            TCCR0B = tccrb.byte;
            TCNT0 = 0;
            break;

        case TIMER_2:
            tccrb.byte = TCCR2B;
            tccrb.bits.CS = timer_cfg[self].prescaler;
            TCCR2B = tccrb.byte;
            TCNT2 = 0;
            break;

        default:
            return -TIMER_ERROR_INSTANCE;
    }

    return TIMER_OK;
}

int timer_stop(timer_t self)
{
    if (self < NUMBER_OF_TIMERS && !timer_cfg[self].is_init)
        return -TIMER_ERROR_NOT_INIT;

    tccrb_t tccrb = { .byte = 0 };

    switch(self)
    {
        case TIMER_0:
            tccrb.byte = TCCR0B;
            tccrb.bits.CS = TIMER_PRESCALER_STOPPED;
            TCCR0B = tccrb.byte;
            break;

        case TIMER_2:
            tccrb.byte = TCCR2B;
            tccrb.bits.CS = TIMER_PRESCALER_STOPPED;
            TCCR2B = tccrb.byte;
            break;

        default:
            return -TIMER_ERROR_INSTANCE;
    }

    return TIMER_OK;
}
