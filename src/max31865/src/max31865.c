#include "max31865.h"
#include "max31865_prv.h"
#include "max31865_cfg.h"
#include "spi.h"
#include "bits.h"
#include "types.h"
#include <math.h>

int max31865_init(max31865_t *self, gpio_t *cs, max31865_mode_t mode, max31865_filter_t filter)
{
    spi_device_init(&self->dev, cs, SPI_CLOCK_DIV_16 , SPI_MODE_3);
    spi_device_set_transaction_mode(&self->dev, TRUE);

    // VBIAS = On, Conversion mode = Auto
    uint8_t configuration = BIT(7) | BIT(6);
    uint8_t read = 0;

    switch(mode)
    {
        case MAX31865_MODE_2_WIRES:
            // Nothing to do
            break;

        case MAX31865_MODE_3_WIRES:
            SET_BIT(configuration, 4);
            break;

        case MAX31865_MODE_4_WIRES:
            // Nothing to do
            break;
    }

    switch(filter)
    {
        case MAX31865_FILTER_50HZ:
            SET_BIT(configuration, 0);
            break;

        case MAX31865_FILTER_60HZ:
            // Nothing to do
            break;
    }

    // Write config
    spi_device_enable(&self->dev);
    spi_device_write_byte(&self->dev, MAX31865_REG_CONFIG | MAX31865_WRITE, NULL_PTR);
    spi_device_write_byte(&self->dev, configuration, NULL_PTR);
    spi_device_disable(&self->dev);

    // Read back config
    spi_device_enable(&self->dev);
    spi_device_write_byte(&self->dev, MAX31865_REG_CONFIG | MAX31865_READ, NULL_PTR);
    spi_device_write_byte(&self->dev, configuration, &read);
    spi_device_disable(&self->dev);

    // Same config should be read back if communication is set up properly
    if (read != configuration) return -1;

    return 0;
}

int max31865_read_rtd(max31865_t *self, double *rtd, bool *fault)
{
    uint8_t msb = 0;
    uint8_t lsb = 0;

    spi_device_enable(&self->dev);
    spi_device_write_byte(&self->dev, MAX31865_REG_RTD_MSB | MAX31865_READ, NULL_PTR);
    spi_device_read_byte(&self->dev, &msb);
    spi_device_read_byte(&self->dev, &lsb);
    spi_device_disable(&self->dev);

    if ( lsb == 0 && msb == 0 )
    {
        // Device disconnected, probably...
        return -1;
    }

    *fault = IS_SET_BIT(lsb, 0) ? TRUE : FALSE;

    uint16_t w = (msb << 8) | lsb;
    *rtd = (MAX31865_RES_REF * TYPECAST(w >> 1, double)) / MAX31865_ADC_RESOLUTION;

    return 0;
}

// Implementation of Callendar-Van Dusen equation
// Only for temperatures > 0°C
double max31865_rtd_to_temperature(double rtd)
{
    double acc = 0;
    acc = rtd/MAX31865_RTD_RES_0;
    acc = 1 - acc;
    acc = 4 * MAX31865_RTD_B * acc;
    acc = (MAX31865_RTD_A * MAX31865_RTD_A) - acc;
    acc = sqrt(acc) - MAX31865_RTD_A;
    return acc / (2 * MAX31865_RTD_B);
}

int max31865_read_fault_status(max31865_t *self, uint8_t *status)
{
    spi_device_enable(&self->dev);
    spi_device_write_byte(&self->dev, MAX31865_REG_FAULT_STATUS | MAX31865_READ, NULL_PTR);
    spi_device_write_byte(&self->dev, 0, status);
    spi_device_disable(&self->dev);

    return 0;
}
