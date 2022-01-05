#include "pmbus.h"
#include "i2c.h"
#include "bits.h"
#include "math.h"

/*
 * Private defines
 */

#define REG_VOUT_MODE           0x20
#define REG_READ_TEMPERATURE_1  0x8D
#define REG_READ_TEMPERATURE_2  0x8E
#define REG_READ_TEMPERATURE_3  0x8F
#define REG_READ_VIN            0x88
#define REG_READ_IIN            0x89
#define REG_READ_VOUT           0x8B
#define REG_READ_IOUT           0x8C
#define REG_READ_POUT           0x96
#define REG_READ_PIN            0x97
#define REG_PMBUS_REVISION      0x98
#define REG_MFR_MODEL           0x9A
#define REG_MFR_BLACKBOX        0xDC
#define REG_MFR_BLACKBOX_CONFIG 0xDF

/*
 * Private functions declaration
 */

static int pmbus_read_word_linear11(pmbus_t self, uint8_t reg, double *value);
static int pmbus_read_string(pmbus_t self, uint8_t reg, char *buf, unsigned int sz);
static int twos_complement(unsigned int n, unsigned int b);

/*
 * Public methods
 */

int pmbus_init(pmbus_t self, i2c_bus_t bus, uint8_t addr)
{
    return i2c_device_init(&self->dev, bus, addr);
}

int pmbus_read_mfr_model(pmbus_t self, char *buffer)
{
    return pmbus_read_string(self, REG_MFR_MODEL, buffer, PMBUS_MFR_MODEL_MAX_LENGTH);
}

int pmbus_read_temperature(pmbus_t self, double *temperature)
{
    return pmbus_read_word_linear11(self, REG_READ_TEMPERATURE_1, temperature);
}

int pmbus_read_vin(pmbus_t self, double *vin)
{
    return pmbus_read_word_linear11(self, REG_READ_VIN, vin);
}

int pmbus_read_iin(pmbus_t self, double *vin)
{
    return pmbus_read_word_linear11(self, REG_READ_IIN, vin);
}

int pmbus_read_pin(pmbus_t self, double *vin)
{
    return pmbus_read_word_linear11(self, REG_READ_PIN, vin);
}

int pmbus_read_iout(pmbus_t self, double *iout)
{
    return pmbus_read_word_linear11(self, REG_READ_IOUT, iout);
}

int pmbus_read_vout(pmbus_t self, double *vout)
{
    uint16_t data = 0;
    int res = 0;

    res = i2c_device_read_bytes(&self->dev, REG_READ_VOUT, &data, 2);
    if (res < 0) return -1;

    res = pmbus_vout_decode(self, data, vout);
    if (res < 0) return -2;

    return 0;
}

int pmbus_read_blackbox(pmbus_t self, struct blackbox_data_s * data)
{
    return i2c_device_read_bytes(&self->dev, REG_MFR_BLACKBOX, data, sizeof(*data));
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
    uint8_t vout_mode = 0;
    uint8_t vout_mode_m, vout_mode_p;
    int res = 0;

    // TODO : read only once ?
    res = i2c_device_read_byte(&self->dev, REG_VOUT_MODE, &vout_mode);
    if (res < 0) return res;

    vout_mode_m = (vout_mode >> 5) & 0x7; // 3 highest bits
    vout_mode_p = (vout_mode) & 0x1F;     // 5 lowest bits

    if(vout_mode_m == 0)
    {
        int exponent = twos_complement(vout_mode_p, 5);
        *value = raw * pow(2, exponent);
        return 0;
    }

    return -1;
}

/*
 * Private methods
 */

static int pmbus_read_word_linear11(pmbus_t self, uint8_t reg, double *value)
{
    uint16_t data = 0;

    int res = i2c_device_read_bytes(&self->dev, reg, &data, 2);
    if (res < 0) return -1;

    *value = pmbus_linear11_decode(data);
    return 0;
}

static int pmbus_read_string(pmbus_t self, uint8_t reg, char *buf, unsigned int sz)
{
    int res = i2c_device_read_bytes(&self->dev, reg, buf, sz);
    if (res < 0) return -1;

    // First byte is string length
    unsigned int len = TYPECAST(buf[0], unsigned int);
    if (len > sz) return -2;

    // Shift all characters
    for (unsigned int i = 0 ; i < len ; i++)
        buf[i] = buf[i+1];

    // Terminate string
    buf[len] = 0;

    return 0;
}

// TODO : make macro
static int twos_complement(unsigned int n, unsigned int b)
{
    if (n & BIT(b-1)) return (int) n - (int) BIT(b);
    else return (int) n;
}
