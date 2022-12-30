#include "adc.h"
#include "types.h"
#include "macros.h"
#include "os_mem.h"
#include <string.h>

#if OS_MALLOC
adc_handle_t adc_create(void)
{
    adc_handle_t self = os_malloc(sizeof(*self));
    return self;
}

void adc_destroy(adc_handle_t dev)
{
    os_free(dev);
}
#endif // OS_MALLOC

int adc_init(adc_handle_t self, adc_t adc)
{
    CHECK_NULL(self);

    memset(self, 0, sizeof(*self));

    adc_ll_init(adc);

    self->dev = adc;
    self->configured = false;
    self->started = false;

    return ADC_OK;
}


int adc_configure(adc_handle_t self, adc_vref_t vref, adc_prescaler_t pscl)
{
    CHECK_NULL(self);

    uint8_t vref_ll = 0;
    uint8_t pscl_ll = 0;

    switch(vref)
    {
        case ADC_VREF_AREF:        vref_ll = ADC_LL_VREF_AREF;        break;
        case ADC_VREF_AVCC:        vref_ll = ADC_LL_VREF_AVCC;        break;
        case ADC_VREF_INTERNAL1V1: vref_ll = ADC_LL_VREF_INTERNAL1V1; break;
        default: return -ADC_ERR_PARAM;
    }

    switch(pscl)
    {
        case ADC_PRESCALER_2:   pscl_ll = ADC_LL_PSCL_2;   break;
        case ADC_PRESCALER_4:   pscl_ll = ADC_LL_PSCL_4;   break;
        case ADC_PRESCALER_8:   pscl_ll = ADC_LL_PSCL_8;   break;
        case ADC_PRESCALER_16:  pscl_ll = ADC_LL_PSCL_16;  break;
        case ADC_PRESCALER_32:  pscl_ll = ADC_LL_PSCL_32;  break;
        case ADC_PRESCALER_64:  pscl_ll = ADC_LL_PSCL_64;  break;
        case ADC_PRESCALER_128: pscl_ll = ADC_LL_PSCL_128; break;
        default: return -ADC_ERR_PARAM;
    }

    adc_ll_set_vref(self->dev, vref_ll);
    adc_ll_set_prescaler(self->dev, pscl_ll);

    self->configured = true;

    return ADC_OK;
}

int adc_read_word(adc_handle_t self, adc_channel_t channel, uint16_t * value)
{
    CHECK_NULL(self);

    if (!self->configured)
        return -ADC_ERR_INIT;

    if (self->started)
        return -ADC_ERR_BUSY;

    if(channel >= NUMBER_OF_ADC_CHANNELS)
        return -ADC_ERR_PARAM;

    adc_ll_select_channel(self->dev, channel);
    adc_ll_set_left_aligned(self->dev, false);
    adc_ll_set_enabled(self->dev, true);
    adc_ll_start_conversion(self->dev);
    adc_ll_wait_conversion(self->dev);

    *value = adc_ll_read_word(self->dev);

    adc_ll_set_enabled(self->dev, false);

    return ADC_OK;
}

int adc_read_byte(adc_handle_t self, adc_channel_t channel, uint8_t * value)
{
    CHECK_NULL(self);

    if (!self->configured)
        return -ADC_ERR_INIT;

    if (self->started)
        return -ADC_ERR_BUSY;

    if(channel >= NUMBER_OF_ADC_CHANNELS)
        return -ADC_ERR_PARAM;

    adc_ll_select_channel(self->dev, channel);
    adc_ll_set_left_aligned(self->dev, true);
    adc_ll_set_enabled(self->dev, true);
    adc_ll_start_conversion(self->dev);
    adc_ll_wait_conversion(self->dev);

    *value = adc_ll_read_byte(self->dev);

    adc_ll_set_enabled(self->dev, false);

    return ADC_OK;
}

int adc_setup_trigger(adc_handle_t self, adc_trigger_t trigger)
{
    CHECK_NULL(self);

    uint8_t trg = 0;

    if (!self->configured)
        return -ADC_ERR_INIT;

    if (self->started)
        return -ADC_ERR_BUSY;

    switch(trigger)
    {
        case ADC_TRIGGER_DISABLED:               trg = ADC_LL_TRIG_SRC_FREE_RUNNING; break;
        case ADC_TRIGGER_FREE_RUNNING:           trg = ADC_LL_TRIG_SRC_FREE_RUNNING; break;
        case ADC_TRIGGER_ANALOG_COMPARATOR:      trg = ADC_LL_TRIG_SRC_ANALOG_COMP;  break;
        case ADC_TRIGGER_EXTERNAL_IRQ_0:         trg = ADC_LL_TRIG_SRC_EXT_IRQ;      break;
        case ADC_TRIGGER_TIMER0_COMPARE_MATCH_A: trg = ADC_LL_TRIG_SRC_TIM0_COMA;    break;
        case ADC_TRIGGER_TIMER0_OVERFLOW:        trg = ADC_LL_TRIG_SRC_TIM0_OVF;     break;
        case ADC_TRIGGER_TIMER1_COMPARE_MATCH_B: trg = ADC_LL_TRIG_SRC_TIM1_COMB;    break;
        case ADC_TRIGGER_TIMER1_OVERFLOW:        trg = ADC_LL_TRIG_SRC_TIM1_OVF;     break;
        case ADC_TRIGGER_TIMER1_CAPTURE_EVENT:   trg = ADC_LL_TRIG_SRC_TIM1_EVT;     break;
        default: return -ADC_ERR_PARAM;
    }

    adc_ll_set_trigger_source(self->dev, trg);

    if(trigger == ADC_TRIGGER_DISABLED)
    {
        adc_ll_set_auto_trigger(self->dev, false);
        adc_ll_set_interrupts_enabled(self->dev, false);
        adc_ll_set_enabled(self->dev, false);
        self->started = false;
    }
    else
    {
        adc_ll_set_auto_trigger(self->dev, true);
        adc_ll_set_interrupts_enabled(self->dev, true);
        adc_ll_set_enabled(self->dev, true);
        self->started = true;
    }

    return ADC_OK;
}
