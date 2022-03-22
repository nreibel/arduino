#include "adc.h"
#include "types.h"

int adc_init(adc_handle_t self, adc_t adc)
{
    adc_ll_init(adc);

    self->dev = adc;
    self->configured = FALSE;

    return ADC_OK;
}


int adc_configure(adc_handle_t self, adc_vref_t vref, adc_prescaler_t pscl)
{
    if (self == NULL_PTR)
        return -ADC_ERROR_INSTANCE;

    switch(vref)
    {
        case ADC_VREF_AREF:
            adc_ll_set_vref(self->dev, ADC_LL_VREF_AREF);
            break;

        case ADC_VREF_AVCC:
            adc_ll_set_vref(self->dev, ADC_LL_VREF_AVCC);
            break;

        case ADC_VREF_INTERNAL1V1:
            adc_ll_set_vref(self->dev, ADC_LL_VREF_INTERNAL1V1);
            break;

        default:
            return -ADC_ERROR_VREF;
    }

    switch(pscl)
    {
        case ADC_PRESCALER_2:
            adc_ll_set_prescaler(self->dev, ADC_LL_PSCL_2);
            break;

        case ADC_PRESCALER_4:
            adc_ll_set_prescaler(self->dev, ADC_LL_PSCL_4);
            break;

        case ADC_PRESCALER_8:
            adc_ll_set_prescaler(self->dev, ADC_LL_PSCL_8);
            break;

        case ADC_PRESCALER_16:
            adc_ll_set_prescaler(self->dev, ADC_LL_PSCL_16);
            break;

        case ADC_PRESCALER_32:
            adc_ll_set_prescaler(self->dev, ADC_LL_PSCL_32);
            break;

        case ADC_PRESCALER_64:
            adc_ll_set_prescaler(self->dev, ADC_LL_PSCL_64);
            break;

        case ADC_PRESCALER_128:
            adc_ll_set_prescaler(self->dev, ADC_LL_PSCL_128);
            break;

        default:
            return -ADC_ERROR_PRESCALER;
    }

    self->configured = TRUE;

    return ADC_OK;
}

int adc_read_word(adc_handle_t self, adc_channel_t channel, uint16_t * value)
{
    if (self == NULL_PTR)
        return -ADC_ERROR_INSTANCE;

    if (!self->configured)
        return -ADC_ERROR_NOT_CONFIGURED;

    switch(channel)
    {
        case ADC_CHANNEL_0:
        case ADC_CHANNEL_1:
        case ADC_CHANNEL_2:
        case ADC_CHANNEL_3:
        case ADC_CHANNEL_4:
        case ADC_CHANNEL_5:
        case ADC_CHANNEL_6:
        case ADC_CHANNEL_7:
            adc_ll_select_channel(self->dev, channel);
            break;

        default:
            return -ADC_ERROR_CHANNEL;
    }

    adc_ll_set_left_aligned(self->dev, FALSE);

    adc_ll_set_enabled(self->dev, TRUE);
    adc_ll_start_conversion(self->dev);
    adc_ll_wait_conversion(self->dev);

    *value = adc_ll_read_word(self->dev);

    adc_ll_set_enabled(self->dev, FALSE);

    return ADC_OK;
}

int adc_read_byte(adc_handle_t self, adc_channel_t channel, uint8_t * value)
{
    if (self == NULL_PTR)
        return -ADC_ERROR_INSTANCE;

    if (!self->configured)
        return -ADC_ERROR_NOT_CONFIGURED;

    switch(channel)
    {
        case ADC_CHANNEL_0:
        case ADC_CHANNEL_1:
        case ADC_CHANNEL_2:
        case ADC_CHANNEL_3:
        case ADC_CHANNEL_4:
        case ADC_CHANNEL_5:
        case ADC_CHANNEL_6:
        case ADC_CHANNEL_7:
            adc_ll_select_channel(self->dev, channel);
            break;

        default:
            return -ADC_ERROR_CHANNEL;
    }

    adc_ll_set_left_aligned(self->dev, TRUE);

    adc_ll_set_enabled(self->dev, TRUE);
    adc_ll_start_conversion(self->dev);
    adc_ll_wait_conversion(self->dev);

    *value = adc_ll_read_byte(self->dev);

    adc_ll_set_enabled(self->dev, FALSE);

    return ADC_OK;
}
