#include "i2c.h"
#include "ltc4260.h"
#include "bits.h"
#include "types.h"
#include "os.h"
#include "os_mem.h"

/*
 * Private constants
 */

enum {
    LTC4260_REG_CONTROL,
    LTC4260_REG_ALERT,
    LTC4260_REG_STATUS,
    LTC4260_REG_FAULT,
    LTC4260_REG_SENSE,
    LTC4260_REG_SOURCE,
    LTC4260_REG_ADIN,
};

enum {
    LTC4260_CTRL_OV_AR, // Overvoltage Autoretry
    LTC4260_CTRL_UV_AR, // Undervoltage Autoretry
    LTC4260_CTRL_OC_AR, // Overcurrent Autoretry
};

#define LTC4260_SENSE_FULL_SCALE  0.0768 // 300µV/LSB = 76,8mV
#define LTC4260_SOURCE_FULL_SCALE 102.4  // 400mV/LSB = 102,4V
#define LTC4260_ADIN_FULL_SCALE   2.56   // 10mV/LSB = 2.56V
/*
 * Exported functions
 */

int ltc4260_read_sense(ltc4260_t self, double * value, unsigned int timeout)
{
    uint8_t buf = LTC4260_REG_SENSE;

    int err = i2c_device_transaction(&self->super, &buf, 1, &buf, 1, timeout);
    if (err < 0) return -LTC4260_ERR_BUS;

    *value = (buf / 255.0) * LTC4260_SENSE_FULL_SCALE;

    return LTC4260_OK;
}

int ltc4260_read_source(ltc4260_t self, double * value, unsigned int timeout)
{
    uint8_t buf = LTC4260_REG_SOURCE;

    int err = i2c_device_transaction(&self->super, &buf, 1, &buf, 1, timeout);
    if (err < 0) return -LTC4260_ERR_BUS;

    *value = (buf / 255.0) * LTC4260_SOURCE_FULL_SCALE;

    return LTC4260_OK;
}

int ltc4260_read_adin(ltc4260_t self, double * value, unsigned int timeout)
{
    uint8_t buf = LTC4260_REG_ADIN;

    int err = i2c_device_transaction(&self->super, &buf, 1, &buf, 1, timeout);
    if (err < 0) return -LTC4260_ERR_BUS;

    *value = (buf / 255.0) * LTC4260_ADIN_FULL_SCALE;

    return LTC4260_OK;
}

int ltc4260_set_overcurrent_autoretry(ltc4260_t self, bool enabled, unsigned int timeout)
{
    if (enabled) SET_BIT(self->ctrl, LTC4260_CTRL_OC_AR);
    else RESET_BIT(self->ctrl, LTC4260_CTRL_OC_AR);

    const uint8_t data[2] = { LTC4260_REG_CONTROL, self->ctrl };
    int err = i2c_device_transaction(&self->super, data, 2, NULL_PTR, 0, timeout);
    if (err < 0) return -LTC4260_ERR_BUS;

    return LTC4260_OK;
}

int ltc4260_set_overvoltage_autoretry(ltc4260_t self, bool enabled, unsigned int timeout)
{
    if (enabled) SET_BIT(self->ctrl, LTC4260_CTRL_OV_AR);
    else RESET_BIT(self->ctrl, LTC4260_CTRL_OV_AR);

    const uint8_t data[2] = { LTC4260_REG_CONTROL, self->ctrl };
    int err = i2c_device_transaction(&self->super, data, 2, NULL_PTR, 0, timeout);
    if (err < 0) return -LTC4260_ERR_BUS;

    return LTC4260_OK;
}

int ltc4260_set_undervoltage_autoretry(ltc4260_t self, bool enabled, unsigned int timeout)
{
    if (enabled) SET_BIT(self->ctrl, LTC4260_CTRL_UV_AR);
    else RESET_BIT(self->ctrl, LTC4260_CTRL_UV_AR);

    const uint8_t data[2] = { LTC4260_REG_CONTROL, self->ctrl };
    int err = i2c_device_transaction(&self->super, data, 2, NULL_PTR, 0, timeout);
    if (err < 0) return -LTC4260_ERR_BUS;

    return LTC4260_OK;
}

#if OS_MALLOC

ltc4260_t ltc4260_create(i2c_bus_t parent, uint8_t addr, unsigned int timeout)
{
    ltc4260_t self = os_malloc(sizeof(*self));

    if (self == NULL_PTR)
        goto exit;

    if (ltc4260_init(self, parent, addr, timeout) < 0)
        goto cleanup;

    return self;

    cleanup:
        os_free(self);

    exit:
        return NULL_PTR;
}

void ltc4260_destroy(ltc4260_t self)
{
    if (self != NULL_PTR)
    {
        os_free(self);
    }
}

#endif // OS_MALLOC

int ltc4260_init(ltc4260_t self, i2c_bus_t parent, uint8_t addr, unsigned int timeout)
{
    int err = 0;

    // Call superclass constructor
    err = i2c_device_init(&self->super, parent, addr);
    if (err < 0) return err;

    // Read control register
    const uint8_t ctrl = LTC4260_REG_CONTROL;
    err = i2c_device_transaction(&self->super, &ctrl, 1, &self->ctrl, 1, timeout);
    if (err < 0) return -LTC4260_ERR_BUS;

    return LTC4260_OK;
}
