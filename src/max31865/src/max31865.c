#include "max31865.h"
#include "max31865_cfg.h"
#include "spi.h"
#include "bits.h"
#include "types.h"
#include "os_mem.h"
#include "macros.h"

#include <string.h>
#include <math.h>

/*
 * Private constants
 */
#define MAX31865_REG_CONFIG       0x00
#define MAX31865_REG_RTD_MSB      0x01
#define MAX31865_REG_RTD_LSB      0x02
#define MAX31865_REG_HFT_MSB      0x03
#define MAX31865_REG_HFT_LSB      0x04
#define MAX31865_REG_LFT_MSB      0x05
#define MAX31865_REG_LFT_LSB      0x06
#define MAX31865_REG_FAULT_STATUS 0x07

#define MAX31865_READ  0x00
#define MAX31865_WRITE 0x80

#define MAX31865_ADC_RESOLUTION (1ULL << 15)

/*
 * Exported functions
 */

#if OS_MALLOC
max31865_t max31865_create(void)
{
    max31865_t self = os_malloc(sizeof(*self));
    return self;
}

void max31865_destroy(max31865_t dev)
{
    os_free(dev);
}
#endif // OS_MALLOC

int max31865_init(max31865_t self, spi_bus_t bus, gpio_t cs, max31865_mode_t mode, max31865_filter_t filter)
{
    CHECK_NULL(self);

    int ret = 0;

    memset(self, 0, sizeof(*self));

    ret = spi_device_init(&self->dev, bus, cs, SPI_CLOCK_DIV_16, SPI_MODE_3);
    if (ret < 0) return -MAX31865_ERR_INIT;

    // VBIAS = On, Conversion mode = Auto
    uint8_t configuration = BIT(7) | BIT(6);

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

        default:
            return -MAX31865_ERR_PARAM;
    }

    switch(filter)
    {
        case MAX31865_FILTER_50HZ:
            SET_BIT(configuration, 0);
            break;

        case MAX31865_FILTER_60HZ:
            // Nothing to do
            break;

        default:
            return -MAX31865_ERR_PARAM;
    }

    // Write config
    uint8_t data_wr[] = { MAX31865_REG_CONFIG | MAX31865_WRITE, configuration };
    ret += spi_device_transfer(&self->dev, data_wr, data_wr, sizeof(data_wr));

    // Read back config
    uint8_t data_rd[] = { MAX31865_REG_CONFIG | MAX31865_READ, 0 };
    ret += spi_device_transfer(&self->dev, data_rd, data_rd, sizeof(data_rd));

    // Same config should be read back if communication is set up properly
    if (ret != 4 || data_rd[1] != configuration)
        return -MAX31865_ERR_INIT;

    return MAX31865_OK;
}

int max31865_read_rtd(max31865_t self, double *rtd)
{
    CHECK_NULL(self);

    int ret = 0;
    uint8_t data[] = { MAX31865_REG_RTD_MSB | MAX31865_READ, 0, 0 };

    ret = spi_device_transfer(&self->dev, data, data, 3);
    if (ret != 3) return -MAX31865_FAIL;

    const uint8_t msb = data[1];
    const uint8_t lsb = data[2];

    if (lsb == 0 && msb == 0)
        return -MAX31865_ERR_DISCONNECTED;

    if (lsb & 0x1)
        return -MAX31865_ERR_FAULT;

    uint16_t w = (msb << 8) | lsb;
    *rtd = (MAX31865_RES_REF * TYPECAST(w >> 1, double)) / MAX31865_ADC_RESOLUTION;

    return MAX31865_OK;
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

int max31865_read_fault_status(max31865_t self, uint8_t *status)
{
    CHECK_NULL(self);

    int ret = 0;
    uint8_t data[] = { MAX31865_REG_FAULT_STATUS | MAX31865_READ, 0 };

    ret += spi_device_transfer(&self->dev, data, data, sizeof(data));
    if (ret != 2) return -MAX31865_FAIL;

    *status = data[1];

    return MAX31865_OK;
}
