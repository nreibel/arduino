#include "stdio.h"
#include "os.h"
#include "app.h"
#include "gpio.h"
#include "serial.h"
#include "icp.h"
#include "i2c.h"
#include "i2c_drv.h"
#include "tc74.h"
#include "adc_ll.h"
#include "timer.h"

#if defined __AVR_ATmega32U4__ // Leonardo
static const usart_t usart = USART1;
#elif defined __AVR_ATmega328P__ // Uno
static const usart_t usart = USART0;
#else
#error "Unknown architecture"
#endif

static struct icp_handle_prv_s icp_data;
static icp_handle_t icp = &icp_data;

static gpio_t led = NULL_PTR;
static tc74_t tc74 = NULL_PTR;
static i2c_bus_t i2c = NULL_PTR;

int os_putc(char chr, FILE *stream)
{
    UNUSED(stream);
    serial_write_byte(usart, chr);
    return chr;
}

void extint_cbk(extint_t pin, volatile void * data)
{
    UNUSED(data);
    printf("ext int %u\r\n", pin);
}

void pcint_cbk(pcint_t port, uint8_t mask, volatile void * data)
{
    UNUSED(data);
    printf("pcint %u port=0x%02X\r\n", port, mask);
}

void adc_interrupt(adc_t adc, uint16_t value)
{
    adc_ll_set_enabled(adc, FALSE);

    unsigned long int mv = (value * 5000UL) / ADC_MAX_VALUE;
    printf("adc read 0x%04x (%lumV)\r\n", value, mv);
}

// App entry point
void app_init()
{
    serial_init(usart, 19200);

    i2c = i2c_driver_create(TWI0, FALSE);
    tc74 = tc74_create(i2c, TC74A4);

#if defined __AVR_ATmega32U4__ // Leonardo
    led = gpio_create(PORT_C, 7);
    gpio_enable_extint(EXTINT6, GPIO_EDGE_RISING, extint_cbk, NULL_PTR);
#elif defined __AVR_ATmega328P__ // Uno
    led = gpio_create(PORT_B, 5);
    gpio_enable_extint(EXTINT0, GPIO_EDGE_RISING, extint_cbk, NULL_PTR);
#endif

    gpio_set_data_direction(led, GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_enable_pcint(PCINTB, 0x0E, pcint_cbk, NULL_PTR);

    // Setup ADC
    adc_ll_init(ADC0);
    adc_ll_set_prescaler(ADC0, ADC_LL_PSCL_128);
    adc_ll_set_vref(ADC0, ADC_LL_VREF_AVCC);
    adc_ll_select_channel(ADC0, 0);
    adc_ll_set_interrupts_enabled(ADC0, TRUE);

    // Setup PWM on Timer2 : 200Hz, 50% Duty Cycle
    timer_config_t timer_cfg = {
        .mode = TIMER_MODE_FAST_PWM,
        .output_mode_b = TIMER_OUTPUT_MODE_DEFAULT,
        .oca_mode = TIMER_OCA_MODE_TOP,
        .prescaler = TIMER_PRESCALER_1024,
        .ocra = 77,
        .ocrb = 38,
    };

    timer_init(TIMER2, &timer_cfg);
    timer_start(TIMER2);

    /* Use blocking mode */
    if (icp_init(icp, ICP1, ICP_PRESCALER_256) < 0)
    {
        printf("icp init error\r\n");
        HALT();
    }

    printf("Start!\r\n");

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);
}

void serial_rx_callback(usart_t usart, const char *buffer, unsigned int length)
{
    UNUSED(usart);
    printf("received %u bytes : [%s]\r\n", length, buffer);
}

void serial_rx_overflow(usart_t usart)
{
    UNUSED(usart);
    printf("usart overflow!\r\n");
}

// Main task
int task_main(void * data)
{
    UNUSED(data);

    int ret = 0;
    int temperature = 0;
    uint8_t duty_cycle = 0;
    uint16_t freq = 0;

    gpio_toggle(led);

    static int cpt = 0;
    if (cpt++ & 1) printf("tock\r\n");
    else printf("tick\r\n");

    ret = tc74_read_temperature(tc74, &temperature);
    if (ret < 0) printf("temperature : read error %d\r\n", ret);
    else printf("temperature : %d°C\r\n", temperature);

    ret = icp_get_duty_cycle(icp, &duty_cycle);
    if (ret < 0) printf("icp error %d\r\n", ret);
    else
    {
        uint8_t percent = (duty_cycle * 100UL) / 0xFF;
        printf("duty cycle is %u%%\r\n", percent);
    }

    ret = icp_get_frequency(icp, &freq);
    if (ret < 0) printf("icp error %d\r\n", ret);
    else printf("frequency is %uHz\r\n", freq);

    // Start ADC conversion
    adc_ll_set_enabled(ADC0, TRUE);
    adc_ll_start_conversion(ADC0);

    return 0;
}
