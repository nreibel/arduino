#include "tft_shield.h"
#include "bits.h"
#include "string.h"
#include "os_mem.h"

/*
 * Private constants
 */

#define I2C_SEESAW     0x2E

#define MODULE_PWM     0x08
#define PWM_VAL        0x01

#define MODULE_GPIO    0x01
#define GPIO_DIRSET    0x02
#define GPIO_DIRCLR    0x03
#define GPIO_GPIO      0x04
#define GPIO_SET       0x05
#define GPIO_CLR       0x06
#define GPIO_TOGGLE    0x07
#define GPIO_INTENSET  0x08
#define GPIO_INTENCLR  0x09
#define GPIO_INTFLAG   0x0A
#define GPIO_PULLENSET 0x0B
#define GPIO_PULLENCLR 0x0C

/*
 * Public methods
 */

static struct gpio_prv_s D10 = {
    .port = PORT_B,
    .pin = 2
};

static struct gpio_prv_s D8 = {
    .port = PORT_B,
    .pin = 0
};

#if OS_MALLOC

tft_shield_t tft_shield_create(spi_bus_t spi, i2c_bus_t i2c, st7735_orientation_t orientation)
{
    tft_shield_t self = os_malloc(sizeof(*self));

    if (self == NULL_PTR)
        goto exit;

    if (tft_shield_init(self, spi, i2c, orientation) != 0)
        goto cleanup;

    return self;

    cleanup:
        os_free(self);

    exit:
        return NULL_PTR;
}

void tft_shield_destroy(tft_shield_t self)
{
    if (self != NULL_PTR)
        os_free(self);
}

#endif // OS_MALLOC

int tft_shield_init(tft_shield_t self, spi_bus_t spi, i2c_bus_t i2c, st7735_orientation_t orientation)
{
    int err = I2C_OK;
    uint8_t w = 160, h = 128;

    // Seesaw chip
    err += i2c_device_init(&self->seesaw, i2c, I2C_SEESAW);

    // TFT
    switch(orientation)
    {
        case ST7735_ORIENTATION_LANDSCAPE:
        case ST7735_ORIENTATION_LANDSCAPE_INV:
            /* Nothing to do */
            break;

        case ST7735_ORIENTATION_PORTRAIT:
        case ST7735_ORIENTATION_PORTRAIT_INV:
            w = 128;
            h = 160;
            break;

        default:
            return -TFT_SHIELD_ERR;
    }

    st7735_init_device(&self->tft, spi, &D10, &D8, w, h);
    st7735_set_orientation(&self->tft, orientation);

    // Enable buttons and joystick as input with pullup
    uint8_t buf[6] = { MODULE_GPIO, 0, 0, 0, 0x4F, 0xE0 };

    buf[1] = GPIO_DIRCLR;
    err += i2c_device_transaction(&self->seesaw, buf, 6, 0, 10);

    buf[1] = GPIO_PULLENSET;
    err += i2c_device_transaction(&self->seesaw, buf, 6, 0, 10);

    buf[1] = GPIO_SET;
    err += i2c_device_transaction(&self->seesaw, buf, 6, 0, 10);

    if (err != I2C_OK)
        return -TFT_SHIELD_ERR_BUS;

    return TFT_SHIELD_OK;
}

st7735_t tft_shield_get_tft(tft_shield_t self)
{
    return &self->tft;
}

int tft_shield_backlight(tft_shield_t self, uint8_t val)
{
    uint8_t cmd[4] = { MODULE_PWM, PWM_VAL, 0, val };

    int ret = i2c_device_transaction(&self->seesaw, cmd, 4, 0, 10);

    if (ret != I2C_OK)
        return -TFT_SHIELD_ERR_BUS;

    return TFT_SHIELD_OK;
}

int tft_shield_read_keys(tft_shield_t self, tft_shield_key_t *key)
{
    int res = I2C_OK;
    uint8_t gpio[4] = { MODULE_GPIO, GPIO_GPIO, 0, 0 };

    res += i2c_device_transaction(&self->seesaw, gpio, 2, 4, 10);

    if (res != I2C_OK)
        return -TFT_SHIELD_ERR_BUS;

    switch(~gpio[2] & 0x4F)
    {
        case 0: /* No key pressed */             break;
        case BIT(0): *key = TFT_SHIELD_KEY_LEFT; break;
        case BIT(1): *key = TFT_SHIELD_KEY_DOWN; break;
        case BIT(2): *key = TFT_SHIELD_KEY_A;    break;
        case BIT(3): *key = TFT_SHIELD_KEY_B;    break;
        case BIT(6): *key = TFT_SHIELD_KEY_C;    break;
        default: /* Multiple key press */        break;
    }

    switch(~gpio[3] & 0xE0)
    {
        case 0: /* No key pressed */              break;
        case BIT(5): *key = TFT_SHIELD_KEY_RIGHT; break;
        case BIT(6): *key = TFT_SHIELD_KEY_UP;    break;
        case BIT(7): *key = TFT_SHIELD_KEY_JOY;   break;
        default: /* Multiple key press */         break;
    }

    return TFT_SHIELD_OK;
}
