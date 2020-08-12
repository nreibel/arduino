#include "adc.h"
#include "adc_cfg.h"
#include "types.h"
#include "bits.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "port.h"
#include "gpio_uno.h"

volatile word data = 0;
volatile bool ready = FALSE;

ISR(ADC_vect)
{
    data = ADC;
    ready = TRUE;
}

void ADC_Init(bool pullUpEnabled)
{
    // Poweron ADC
    RESET_BIT(PRR, PRADC);

    // Disable pull-up
    if (pullUpEnabled)
    {
        DDRC  = 0x00;
        PORTC = 0xFF;
    }
    else
    {
        DDRC  = 0x00;
        PORTC = 0x00;
    }

    // Select Vref
    ADMUX = MASK(ADC_VREF_SELECT, 0x3) << 6;

    // Enable ADC with interrupts, set prescaler
    ADCSRA = BIT(ADEN) /*| BIT(ADIE)*/ | MASK(ADC_PRESCALER_SELECT, 0x7);

    ADCSRB = 0;
}

uint16_t ADC_ReadSync(uint8_t channel)
{
    SET_BITS(ADMUX, channel, 0x0F);
    SET_BIT(ADCSRA, ADSC);

    // Wait for acquisition
    while(IS_SET_BIT(ADCSRA, ADSC));

    return ADC;
}
