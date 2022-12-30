#include "adc_ll.h"
#include "types.h"

#include <avr/power.h>
#include <avr/interrupt.h>

/*
 * Handle interrupts
 */

__attribute__((weak))
void adc_interrupt(const adc_ll_callback_args_t * args)
{
    UNUSED(args);
}

ISR(ADC_vect)
{
    const adc_ll_callback_args_t args = {
        .adc   = ADC0,
        .value = ADC0->adc.w,
    };

    adc_interrupt(&args);
}

/*
 * Exported functions
 */

void adc_ll_init(adc_t adc)
{
    power_adc_enable();

    adc->adcsra.reg = 0;
    adc->adcsrb.reg = 0;
    adc->admux.reg = 0;
}

void adc_ll_set_enabled(adc_t adc, bool enabled)
{
    adc->adcsra.bits.aden = enabled ? 1 : 0;
}

void adc_ll_set_interrupts_enabled(adc_t adc, bool enabled)
{
    adc->adcsra.bits.adie = enabled ? 1 : 0;
}

void adc_ll_set_prescaler(adc_t adc, adc_ll_pscl_t pscl)
{
    adc->adcsra.bits.adps = pscl;
}

void adc_ll_set_vref(adc_t adc, adc_ll_vref_t vref)
{
    adc->admux.bits.refs = vref;
}

void adc_ll_set_auto_trigger(adc_t adc, bool enabled)
{
    adc->adcsra.bits.adate = enabled ? 1 : 0;
}

void adc_ll_set_trigger_source(adc_t adc, adc_ll_trig_t source)
{
    adc->adcsrb.bits.adts = source;
}

void adc_ll_set_left_aligned(adc_t adc, bool left_aligned)
{
    adc->admux.bits.adlar = left_aligned ? 1 : 0;
}

void adc_ll_select_channel(adc_t adc, uint8_t channel)
{
    adc->admux.bits.mux = channel;
}

void adc_ll_wait_conversion(adc_t adc)
{
    while(adc->adcsra.bits.adsc);
}

void adc_ll_start_conversion(adc_t adc)
{
    adc->adcsra.bits.adsc = 1;
}

uint16_t adc_ll_read_word(adc_t adc)
{
    return adc->adc.w;
}

uint8_t adc_ll_read_byte(adc_t adc)
{
    return adc->adc.b[1];
}
