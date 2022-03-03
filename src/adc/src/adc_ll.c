#include "adc_ll.h"
#include "types.h"

#include <avr/power.h>
#include <avr/interrupt.h>

/*
 * Handle interrupts
 */

__attribute__((weak))
void adc_interrupt(adc_t adc, uint16_t value)
{
    UNUSED(adc);
    UNUSED(value);
}

ISR(ADC_vect)
{
    adc_interrupt(ADC0, ADC0->adc.u16);
}

/*
 * Exported functions
 */

int adc_ll_init(adc_t adc)
{
    power_adc_enable();
    adc->adcsra.reg = 0;
    adc->adcsrb = 0;
    return ADC_LL_OK;
}

int adc_ll_set_enabled(adc_t adc, bool enabled)
{
    adc->adcsra.bits.aden = enabled ? 1 : 0;
    return ADC_LL_OK;
}

int adc_ll_set_interrupts_enabled(adc_t adc, bool enabled)
{
    adc->adcsra.bits.adie = enabled ? 1 : 0;
    return ADC_LL_OK;
}

int adc_ll_set_prescaler(adc_t adc, uint8_t pscl)
{
    adc->adcsra.bits.adps = pscl;
    return ADC_LL_OK;
}

int adc_ll_set_vref(adc_t adc, uint8_t vref)
{
    adc->admux.bits.refs = vref;
    return ADC_LL_OK;
}

int adc_ll_set_left_aligned(adc_t adc, bool left_aligned)
{
    adc->admux.bits.adlar = left_aligned ? 1 : 0;
    return ADC_LL_OK;
}

int adc_ll_select_channel(adc_t adc, uint8_t channel)
{
    adc->admux.bits.mux = channel;
    return ADC_LL_OK;
}

int adc_ll_wait_conversion(adc_t adc)
{
    while(adc->adcsra.bits.adsc);
    return ADC_LL_OK;
}

int adc_ll_start_conversion(adc_t adc)
{
    adc->adcsra.bits.adsc = 1;
    return ADC_LL_OK;
}

int adc_ll_read_word(adc_t adc, uint16_t * data)
{
    *data = adc->adc.u16;
    return ADC_LL_OK;
}

int adc_ll_read_byte(adc_t adc, uint8_t * data)
{
    *data = adc->adc.bytes[1];
    return ADC_LL_OK;
}


