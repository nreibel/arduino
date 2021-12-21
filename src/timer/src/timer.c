#include "timer.h"
#include "bits.h"
#include "avr/io.h"

/*
 * Private types
 */

typedef union {
    struct {
        uint8_t WGM      : 2;
        uint8_t reserved : 2;
        uint8_t COMB     : 2;
        uint8_t COMA     : 2;
    } bits;
    uint8_t byte;
} tccra_t;

typedef union {
    struct {
        uint8_t CS       : 3;
        uint8_t WGM      : 1;
        uint8_t reserved : 2;
        uint8_t FOCB     : 1;
        uint8_t FOCA     : 1;
    } bits;
    uint8_t byte;
} tccrb_t;

/*
 * Private data
 */

struct {
    bool is_init;
    uint8_t prescaler;
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
    uint8_t ddra = 0;
    uint8_t ddrb = 0;

    switch(self)
    {
        case TIMER_0:
        {
            // Check prescaler
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
            // Check prescaler
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
        case TIMER_MODE_NORMAL:            tccra.bits.WGM = 0x0; break;
        case TIMER_MODE_PHASE_CORRECT_PWM: tccra.bits.WGM = 0x1; break;
        case TIMER_MODE_CTC:               tccra.bits.WGM = 0x2; break;
        case TIMER_MODE_FAST_PWM:          tccra.bits.WGM = 0x3; break;
        default: return -TIMER_ERROR_MODE;
    }

    // Set Compare Outpout Mode A
    switch(config->output_mode_a)
    {
        case TIMER_OUTPUT_MODE_DISABLED: tccra.bits.COMA = 0x0; break;
        case TIMER_OUTPUT_MODE_TOGGLE:   tccra.bits.COMA = 0x1; break;
        case TIMER_OUTPUT_MODE_DEFAULT:  tccra.bits.COMA = 0x2; break;
        case TIMER_OUTPUT_MODE_INVERTED: tccra.bits.COMA = 0x3; break;
        default: return -TIMER_ERROR_OUTPUT_MODE;
    }

    // Set Compare Outpout Mode B
    switch(config->output_mode_b)
    {
        case TIMER_OUTPUT_MODE_DISABLED: tccra.bits.COMB = 0x0; break;
        case TIMER_OUTPUT_MODE_TOGGLE:   tccra.bits.COMB = 0x1; break;
        case TIMER_OUTPUT_MODE_DEFAULT:  tccra.bits.COMB = 0x2; break;
        case TIMER_OUTPUT_MODE_INVERTED: tccra.bits.COMB = 0x3; break;
        default: return -TIMER_ERROR_OUTPUT_MODE;
    }

    // Set DDR A
    switch(config->output_mode_a)
    {
        case TIMER_OUTPUT_MODE_TOGGLE:
        case TIMER_OUTPUT_MODE_DEFAULT:
        case TIMER_OUTPUT_MODE_INVERTED:
        {
            switch(self)
            {
                case TIMER_0: SET_BIT(ddra, 6); break;
                case TIMER_2: SET_BIT(ddra, 3); break;
                default: return -TIMER_ERROR_INSTANCE;
            }
            break;
        }

        default:
            // Do nothing
            break;
    }

    // Set DDR B
    switch(config->output_mode_b)
    {
        case TIMER_OUTPUT_MODE_TOGGLE:
        case TIMER_OUTPUT_MODE_DEFAULT:
        case TIMER_OUTPUT_MODE_INVERTED:
        {
            switch(self)
            {
                case TIMER_0: SET_BIT(ddrb, 5); break;
                case TIMER_2: SET_BIT(ddrb, 3); break;
                default: return -TIMER_ERROR_INSTANCE;
            }
            break;
        }

        default:
            // Do nothing
            break;
    }

    // Set WGM02
    switch(config->oca_mode)
    {
        case TIMER_OCA_MODE_PWM: tccrb.bits.WGM = 0x0; break;
        case TIMER_OCA_MODE_TOP: tccrb.bits.WGM = 0x1; break;
        default: return -TIMER_ERROR_OCA_MODE;
    }

    // Write registers
    switch(self)
    {
        case TIMER_0:
            RESET_BIT(PRR, PRTIM0);
            OCR0A = config->ocra;
            OCR0B = config->ocrb;
            TIMSK0 = MASK(config->imask, 0x3);
            TCCR0A = tccra.byte;
            TCCR0B = tccrb.byte;
            SET_MASK(DDRD, ddra|ddrb);
            break;

        case TIMER_2:
            RESET_BIT(PRR, PRTIM2);
            OCR2A = config->ocra;
            OCR2B = config->ocrb;
            TIMSK2 = MASK(config->imask, 0x3);
            TCCR2A = tccra.byte;
            TCCR2B = tccrb.byte;
            SET_MASK(DDRB, ddra);
            SET_MASK(DDRD, ddrb);
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
        case TIMER_0: tccrb.byte = TCCR0B; break;
        case TIMER_2: tccrb.byte = TCCR2B; break;
        default: return -TIMER_ERROR_INSTANCE;
    }

    tccrb.bits.CS = timer_cfg[self].prescaler;

    switch(self)
    {
        case TIMER_0: TCCR0B = tccrb.byte; break;
        case TIMER_2: TCCR2B = tccrb.byte; break;
        default: return -TIMER_ERROR_INSTANCE;
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
        case TIMER_0: tccrb.byte = TCCR0B; break;
        case TIMER_2: tccrb.byte = TCCR2B; break;
        default: return -TIMER_ERROR_INSTANCE;
    }

    tccrb.bits.CS = TIMER_PRESCALER_STOPPED;

    switch(self)
    {
        case TIMER_0: TCCR0B = tccrb.byte; break;
        case TIMER_2: TCCR2B = tccrb.byte; break;
        default: return -TIMER_ERROR_INSTANCE;
    }

    return TIMER_OK;
}
