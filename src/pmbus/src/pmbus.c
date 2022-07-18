#include "pmbus.h"
#include "i2c.h"
#include "bits.h"
#include "i2c_ll.h"
#include "os_mem.h"

/*
 * Private constants
 */

// VOUT Mode
#define VOUT_MODE_LINEAR 0b000
#define VOUT_MODE_VID    0b001
#define VOUT_MODE_DIRECT 0b010

// MFR Specific (Great Wall)
#define MFR_PAGE      0xE4
#define MFR_POS_TOTAL 0xE5
#define MFR_POS_LAST  0xE6

/*
 * Private functions declaration
 */

static int pmbus_write_u8(pmbus_t self, uint8_t reg, uint8_t value);
static int pmbus_read_u8(pmbus_t self, uint8_t reg, uint8_t * value);
static int pmbus_read_u16(pmbus_t self, uint8_t reg, uint16_t * value);
static int pmbus_read_u32(pmbus_t self, uint8_t reg, uint32_t * value);
static int pmbus_read_linear11(pmbus_t self, uint8_t reg, double *value);
static int pmbus_read_block(pmbus_t self, uint8_t reg, void * buf, unsigned int sz);

static double power2(int y, int n);
static int twos_complement(unsigned int n, unsigned int b);

/*
 * Public methods
 */

#if OS_MALLOC
pmbus_t pmbus_create(i2c_bus_t bus, uint8_t addr)
{
    pmbus_t self = os_malloc(sizeof(*self));

    if (self == NULL_PTR)
        goto exit;

    if (i2c_device_init(&self->dev, bus, addr) < 0)
        goto cleanup;

    self->vout_mode = 0xFF;

    return self;

    cleanup:
        os_free(self);

    exit:
        return NULL_PTR;
}

void pmbus_destroy(pmbus_t self)
{
    if (self != NULL_PTR)
        os_free(self);
}
#endif

int pmbus_gw_read_page(pmbus_t self, unsigned int * page)
{
    uint8_t data = 0;
    int res = pmbus_read_u8(self, MFR_PAGE, &data);
    if (res >= 0) *page = data;
    return res;
}

int pmbus_gw_write_page(pmbus_t self, const unsigned int page)
{
    return pmbus_write_u8(self, MFR_PAGE, page);
}

int pmbus_gw_read_pos_total(pmbus_t self, long int * pos)
{
    uint32_t data = 0;
    int res = pmbus_read_u32(self, MFR_POS_TOTAL, &data);
    if (res >= 0) *pos = data;
    return res;
}

int pmbus_gw_read_pos_last(pmbus_t self, long int * pos)
{
    uint32_t data = 0;
    int res = pmbus_read_u32(self, MFR_POS_LAST, &data);
    if (res >= 0) *pos = data;
    return res;
}

int pmbus_init(pmbus_t self, i2c_bus_t bus, uint8_t addr)
{
    self->vout_mode = 0xFF;
    return i2c_device_init(&self->dev, bus, addr);
}

int pmbus_read_fanspeed(pmbus_t self, unsigned int * fanspeed)
{
    double data;
    int res = pmbus_read_linear11(self, CMD_READ_FAN_SPEED_1, &data);
    if (res >= 0) *fanspeed = data;
    return res;
}

int pmbus_read_mfr_model(pmbus_t self, char *buffer, unsigned int len)
{
    int res = pmbus_read_block(self, CMD_MFR_MODEL, buffer, len-1);
    if (res >= 0) buffer[res] = '\0';
    return res;
}

int pmbus_read_temperature(pmbus_t self, double *temperature)
{
    return pmbus_read_linear11(self, CMD_READ_TEMPERATURE_1, temperature);
}

int pmbus_read_vin(pmbus_t self, double *vin)
{
    return pmbus_read_linear11(self, CMD_READ_VIN, vin);
}

int pmbus_read_iin(pmbus_t self, double *iin)
{
    return pmbus_read_linear11(self, CMD_READ_IIN, iin);
}

int pmbus_read_pin(pmbus_t self, double *pin)
{
    return pmbus_read_linear11(self, CMD_READ_PIN, pin);
}

int pmbus_read_iout(pmbus_t self, double *iout)
{
    return pmbus_read_linear11(self, CMD_READ_IOUT, iout);
}

int pmbus_read_vout(pmbus_t self, double * vout)
{
    uint16_t data = 0;
    int res = 0;

    res = pmbus_read_u16(self, CMD_READ_VOUT, &data);
    if (res < 0) return res;

    res = pmbus_vout_decode(self, data, vout);
    if (res < 0) return res;

    return 2;
}

int pmbus_read_pout(pmbus_t self, double *pout)
{
    return pmbus_read_linear11(self, CMD_READ_POUT, pout);
}

int pmbus_read_blackbox(pmbus_t self, pmbus_blackbox_t * data)
{
    return pmbus_read_block(self, CMD_MFR_BLACKBOX, data, sizeof(pmbus_blackbox_t));
}

int pmbus_read_status_word(pmbus_t self, pmbus_status_word_t * value)
{
    return pmbus_read_u16(self, CMD_STATUS_VOUT, &value->raw);
}

int pmbus_read_status_vout(pmbus_t self, pmbus_status_vout_t * value)
{
    return pmbus_read_u8(self, CMD_STATUS_VOUT, &value->raw);
}

int pmbus_read_status_iout(pmbus_t self, pmbus_status_iout_t * value)
{
    return pmbus_read_u8(self, CMD_STATUS_IOUT, &value->raw);
}

int pmbus_read_status_input(pmbus_t self, pmbus_status_input_t * value)
{
    return pmbus_read_u8(self, CMD_STATUS_INPUT, &value->raw);
}

int pmbus_read_status_temperature(pmbus_t self, pmbus_status_temperature_t * value)
{
    return pmbus_read_u8(self, CMD_STATUS_TEMPERATURE, &value->raw);
}

int pmbus_read_status_cml(pmbus_t self, pmbus_status_cml_t * value)
{
    return pmbus_read_u8(self, CMD_STATUS_CML, &value->raw);
}

int pmbus_read_status_fans_1_2(pmbus_t self, pmbus_status_fans_t * value)
{
    return pmbus_read_u8(self, CMD_STATUS_FANS_1_2, &value->raw);
}

double pmbus_linear11_decode(uint16_t data)
{
    linear11_t l = { .raw = data };

    int y = twos_complement(l.decoded.mantissa, 11);
    int n = twos_complement(l.decoded.exponent, 5);

    return power2(y, n);
}

uint16_t pmbus_linear11_encode(double value)
{
    int8_t exp = 0;

    if (ABS(value) > 1023)
    {
        // Encode big values (discard decimals)
        while(ABS(value) > 1023 && exp < 15)
        {
            value /= 2;
            exp++;
        }
    }
    else if (ABS(value) < 512)
    {
        // Encode smaller values (preserve decimals)
        while(ABS(value) < 512 && exp > -15)
        {
            value *= 2;
            exp--;
        }
    }

    linear11_t data = {
        .decoded = {
            // two's complement
            .exponent = exp > 0 ? exp : (1 << 5) + exp,
            .mantissa = value > 0 ? value : (1 << 11) + value
        }
    };

    return data.raw;
}

int pmbus_vout_decode(pmbus_t self, uint16_t raw, double *value)
{
    if (self->vout_mode == 0xFF)
    {
        int res = pmbus_read_u8(self, CMD_VOUT_MODE, &self->vout_mode);
        if (res < 0) return -PMBUS_ERROR_IO;
    }

    vout_mode_t vout_mode = { .raw = self->vout_mode };

    switch(vout_mode.decoded.m)
    {
        case VOUT_MODE_LINEAR:
        {
            int exponent = twos_complement(vout_mode.decoded.p, 5);
            *value = power2(raw, exponent);
            return PMBUS_OK;
        }

        case VOUT_MODE_VID:
        case VOUT_MODE_DIRECT:
        {
            return -PMBUS_NOT_SUPPORTED;
        }

        default:
        {
            return -PMBUS_ERROR;
        }
    }

    return -PMBUS_ERROR;
}

/*
 * Private methods
 */

static int pmbus_write_u8(pmbus_t self, uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    int res = i2c_device_transaction(&self->dev, data, 2, NULL_PTR, 0, 10);
    if (res < 0) return -PMBUS_ERROR_IO;
    return 1;
}

static int pmbus_read_u8(pmbus_t self, uint8_t reg, uint8_t * value)
{
    int res = i2c_device_transaction(&self->dev, &reg, 1, value, 1, 10);
    if (res < 0) return -PMBUS_ERROR_IO;
    return 1;
}

static int pmbus_read_u16(pmbus_t self, uint8_t reg, uint16_t * value)
{
    int res = i2c_device_transaction(&self->dev, &reg, 1, value, 2, 10);
    if (res < 0) return -PMBUS_ERROR_IO;
    return 2;
}

static int pmbus_read_u32(pmbus_t self, uint8_t reg, uint32_t * value)
{
    int res = i2c_device_transaction(&self->dev, &reg, 1, value, 4, 10);
    if (res < 0) return -PMBUS_ERROR_IO;
    return 4;
}

static int pmbus_read_linear11(pmbus_t self, uint8_t reg, double *value)
{
    uint16_t data = 0;
    int res = i2c_device_transaction(&self->dev, &reg, 1, &data, 2, 10);
    if (res < 0) return -PMBUS_ERROR_IO;
    *value = pmbus_linear11_decode(data);
    return 2;
}

static int pmbus_read_block(pmbus_t self, uint8_t reg, void * buf, unsigned int sz)
{
    const twi_t twi = self->dev.bus->instance;
    const uint8_t addr = self->dev.addr;

    unsigned int wr = 0, rd = 0;
    uint8_t *bytes = UINT8_PTR(buf);
    int err = I2C_LL_OK;

    err += i2c_ll_start_condition(twi);
    err += i2c_ll_slave_write(twi, addr);
    wr += i2c_ll_write(twi, reg);
    err += i2c_ll_restart_condition(twi);
    err += i2c_ll_slave_read(twi, addr);

    // First byte is string length
    uint8_t len = 0;
    rd += i2c_ll_read_ack(twi, &len);

    if (err != I2C_LL_OK)
    {
        i2c_ll_reset_bus(twi);
        return -PMBUS_ERROR_IO;
    }

    if (len > sz)
    {
        i2c_ll_reset_bus(twi);
        return -PMBUS_ERROR;
    }

    while(rd < len)
        rd += i2c_ll_read_ack(twi, bytes++);

    rd += i2c_ll_read_nack(twi, bytes++);

    err += i2c_ll_stop_condition(twi);

    if (err != I2C_LL_OK || wr != 1 || rd != len+1u)
        return -PMBUS_ERROR_IO;

    return len;
}

// TODO : make macro
static int twos_complement(unsigned int n, unsigned int b)
{
    return IS_SET_BIT(n, b-1) ? (int) n - (int) BIT(b) : (int) n;
}

// Compute y * 2^n
static double power2(int y, int n)
{
    if (n >= 0) return y * 1 << n;
    else return TYPECAST(y, double) / (1 << -n);
}
