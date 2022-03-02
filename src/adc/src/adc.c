#include "adc.h"
#include "types.h"

#include <avr/power.h>
#include <avr/interrupt.h>

__attribute__((weak))
void adc_interrupt(adc_t adc, uint16_t value)
{
    UNUSED(adc);
    UNUSED(value);
}

ISR(ADC_vect)
{
    adc_interrupt(ADC0, ADC0->adc);
}

int adc_ll_init(adc_t adc)
{
    power_adc_enable();
    adc->adcsra.reg = 0;
    adc->adcsrb = 0;
    return ADC_LL_OK;
}

int adc_ll_enable(adc_t adc)
{
    adc->adcsra.bits.aden = 1;
    return ADC_LL_OK;
}

int adc_ll_disable(adc_t adc)
{
    adc->adcsra.bits.aden = 0;
    return ADC_LL_OK;
}

int adc_ll_enable_interrupts(adc_t adc)
{
    adc->adcsra.bits.adie = 1;
    return ADC_LL_OK;
}

int adc_ll_disable_interrupts(adc_t adc)
{
    adc->adcsra.bits.adie = 0;
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

int adc_ll_read_value(adc_t adc, uint16_t * data)
{
    *data = adc->adc;
    return ADC_LL_OK;
}

// uint16_t adc_ll_read(adc_t adc, uint8_t channel)
// {
//     adc->adcsra.bits.adsc = 1;
//
//     // Wait for acquisition
//     while(adc->adcsra.bits.adsc);
//
//     return adc->adc;
// }
