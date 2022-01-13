#ifndef SRC_TFT_SHIELD_API_H_
#define SRC_TFT_SHIELD_API_H_

#include "types.h"
#include "gpio.h"
#include "i2c.h"
#include "st7735.h"

typedef struct tft_shield_prv_s {
    struct i2c_device_prv_s seesaw;
    struct st7735_prv_s tft;
} * tft_shield_t;

typedef enum {
    TFT_SHIELD_KEY_A,
    TFT_SHIELD_KEY_B,
    TFT_SHIELD_KEY_C,
    TFT_SHIELD_KEY_JOY,
    TFT_SHIELD_KEY_UP,
    TFT_SHIELD_KEY_DOWN,
    TFT_SHIELD_KEY_LEFT,
    TFT_SHIELD_KEY_RIGHT,
    NUMBER_OF_TFT_SHIELD_KEYS
} tft_shield_key_t;

int tft_shield_init(tft_shield_t self, i2c_bus_t i2c, st7735_orientation_t orientation);

int tft_shield_backlight(tft_shield_t self, uint8_t val);
int tft_shield_read_keys(tft_shield_t self, tft_shield_key_t *key);

st7735_t tft_shield_get_tft(tft_shield_t self);

#endif /* SRC_TFT_SHIELD_API_H_ */
