#include "pmbus.h"
#include "i2c.h"
#include "bits.h"
#include "math.h"
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

static int pmbus_read_word_linear11(pmbus_t self, uint8_t reg, double *value);
static int pmbus_read_string(pmbus_t self, uint8_t reg, char *buf, unsigned int sz);
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

int pmbus_gw_read_page(pmbus_t self, unsigned int *page)
{
    uint8_t data = 0;

    if (i2c_device_read_byte(&self->dev, MFR_PAGE, &data) != 1)
        return -PMBUS_ERROR_IO;

    *page = data;
    return PMBUS_OK;
}

int pmbus_gw_write_page(pmbus_t self, const unsigned int page)
{
    uint8_t data = page;

    if (i2c_device_write_byte(&self->dev, MFR_PAGE, data) != 1)
        return -PMBUS_ERROR_IO;

    return PMBUS_OK;
}

int pmbus_gw_read_pos_total(pmbus_t self, long int *pos)
{
    uint32_t data;
    int res = i2c_device_read_bytes(&self->dev, MFR_POS_TOTAL, &data, 4);
    if (res != 4) return -PMBUS_ERROR_IO;
    *pos = data;
    return PMBUS_OK;
}

int pmbus_gw_read_pos_last(pmbus_t self, long int *pos)
{
    uint32_t data = 0;
    int res = i2c_device_read_bytes(&self->dev, MFR_POS_LAST, &data, 4);
    if (res != 4) return -PMBUS_ERROR_IO;
    *pos = data;
    return PMBUS_OK;
}

int pmbus_init(pmbus_t self, i2c_bus_t bus, uint8_t addr)
{
    self->vout_mode = 0xFF;
    return i2c_device_init(&self->dev, bus, addr);
}

int pmbus_read_fanspeed(pmbus_t self, unsigned int *fanspeed)
{
    double data;
    int ret = pmbus_read_word_linear11(self, CMD_READ_FAN_SPEED_1, &data);
    if (ret < 0) return -PMBUS_ERROR_IO;
    *fanspeed = data;
    return PMBUS_OK;
}

int pmbus_read_mfr_model(pmbus_t self, char *buffer, unsigned int len)
{
    int ret = pmbus_read_string(self, CMD_MFR_MODEL, buffer, len);
    if (ret < 0) return -PMBUS_ERROR_IO;
    return PMBUS_OK;
}

int pmbus_read_temperature(pmbus_t self, double *temperature)
{
    return pmbus_read_word_linear11(self, CMD_READ_TEMPERATURE_1, temperature);
}

int pmbus_read_vin(pmbus_t self, double *vin)
{
    return pmbus_read_word_linear11(self, CMD_READ_VIN, vin);
}

int pmbus_read_iin(pmbus_t self, double *vin)
{
    return pmbus_read_word_linear11(self, CMD_READ_IIN, vin);
}

int pmbus_read_pin(pmbus_t self, double *vin)
{
    return pmbus_read_word_linear11(self, CMD_READ_PIN, vin);
}

int pmbus_read_iout(pmbus_t self, double *iout)
{
    return pmbus_read_word_linear11(self, CMD_READ_IOUT, iout);
}

int pmbus_read_vout(pmbus_t self, double *vout)
{
    uint16_t data = 0;
    int res = 0;

    res = i2c_device_read_bytes(&self->dev, CMD_READ_VOUT, &data, 2);
    if (res != 2) return -PMBUS_ERROR_IO;

    return pmbus_vout_decode(self, data, vout);
}

int pmbus_read_blackbox(pmbus_t self, pmbus_blackbox_t * data)
{
    int res = i2c_device_read_bytes(&self->dev, CMD_MFR_BLACKBOX, data, sizeof(pmbus_blackbox_t));
    if (res != sizeof(pmbus_blackbox_t)) return -PMBUS_ERROR_IO;
    return PMBUS_OK;
}


int pmbus_read_status_word(pmbus_t self, pmbus_status_word_t *value)
{
    int res = i2c_device_read_bytes(&self->dev, CMD_STATUS_WORD, &value->raw, 2);
    if (res != 2) return -PMBUS_ERROR_IO;
    return PMBUS_OK;
}

int pmbus_read_status_vout(pmbus_t self, pmbus_status_vout_t *value)
{
    int res = i2c_device_read_byte(&self->dev, CMD_STATUS_VOUT, &value->raw);
    if (res != 1) return -PMBUS_ERROR_IO;
    return PMBUS_OK;
}

int pmbus_read_status_iout(pmbus_t self, pmbus_status_iout_t *value)
{
    int res = i2c_device_read_byte(&self->dev, CMD_STATUS_IOUT, &value->raw);
    if (res != 1) return -PMBUS_ERROR_IO;
    return PMBUS_OK;
}

int pmbus_read_status_input(pmbus_t self, pmbus_status_input_t *value)
{
    int res = i2c_device_read_byte(&self->dev, CMD_STATUS_INPUT, &value->raw);
    if (res != 1) return -PMBUS_ERROR_IO;
    return PMBUS_OK;
}

int pmbus_read_status_temperature(pmbus_t self, pmbus_status_temperature_t *value)
{
    int res = i2c_device_read_byte(&self->dev, CMD_STATUS_TEMPERATURE, &value->raw);
    if (res != 1) return -PMBUS_ERROR_IO;
    return PMBUS_OK;
}

int pmbus_read_status_cml(pmbus_t self, pmbus_status_cml_t *value)
{
    int res = i2c_device_read_byte(&self->dev, CMD_STATUS_CML, &value->raw);
    if (res != 1) return -PMBUS_ERROR_IO;
    return PMBUS_OK;
}

int pmbus_read_status_fans_1_2(pmbus_t self, pmbus_status_fans_t *value)
{
    int res = i2c_device_read_byte(&self->dev, CMD_STATUS_FANS_1_2, &value->raw);
    if (res != 1) return -PMBUS_ERROR_IO;
    return PMBUS_OK;
}

double pmbus_linear11_decode(uint16_t data)
{
    int16_t y = data & 0x7FF;         // 11 lowest bits
    int16_t n = (data >> 11 ) & 0x1F; // 5 highest bits

    int y2 = twos_complement(y, 11);
    int n2 = twos_complement(n, 5);

    return y2 * pow(2, n2);
}

int pmbus_vout_decode(pmbus_t self, uint16_t raw, double *value)
{
    if (self->vout_mode == 0xFF)
    {
        int res = i2c_device_read_byte(&self->dev, CMD_VOUT_MODE, &self->vout_mode);
        if (res != 1) return -PMBUS_ERROR_IO;
    }

    vout_mode_t vout_mode = { .raw = self->vout_mode };

    switch(vout_mode.decoded.m)
    {
        case VOUT_MODE_LINEAR:
        {
            int exponent = twos_complement(vout_mode.decoded.p, 5);
            *value = raw * pow(2, exponent);
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

static int pmbus_read_word_linear11(pmbus_t self, uint8_t reg, double *value)
{
    uint16_t data = 0;
    int res = i2c_device_read_bytes(&self->dev, reg, &data, 2);
    if (res != 2) return -PMBUS_ERROR_IO;
    *value = pmbus_linear11_decode(data);
    return PMBUS_OK;
}

static int pmbus_read_string(pmbus_t self, uint8_t reg, char *buf, unsigned int sz)
{
    const twi_t twi = self->dev.bus->instance;
    const uint8_t addr = self->dev.addr;

    unsigned int wr = 0, rd = 0, len = 0;
    uint8_t *bytes = UINT8_PTR(buf);
    int err = 0;

    err += i2c_ll_start_condition(twi);
    err += i2c_ll_slave_write(twi, addr);
    wr += i2c_ll_write(twi, reg);
    err += i2c_ll_restart_condition(twi);
    err += i2c_ll_slave_read(twi, addr);

    // First byte is string length
    rd += i2c_ll_read_ack(twi, bytes);
    len = READ_PU8(bytes);

    if (len > sz-1)
        return -PMBUS_ERROR;

    while(rd < len)
        rd += i2c_ll_read_ack(twi, bytes++);

    rd += i2c_ll_read_nack(twi, bytes++);

    err += i2c_ll_stop_condition(twi);

    // Terminate string
    *bytes = '\0';

    if (err != I2C_OK || wr != 1 || rd != len+1)
        return -PMBUS_ERROR_IO;

    return len;
}

// TODO : make macro
static int twos_complement(unsigned int n, unsigned int b)
{
    return IS_SET_BIT(n, b-1) ? (int) n - (int) BIT(b) : (int) n;
}
