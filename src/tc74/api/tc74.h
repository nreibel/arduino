#ifndef SRC_TC74_API_TC74_H_
#define SRC_TC74_API_TC74_H_

#include "types.h"

typedef enum {
    TC74A0 = 0x48,
    TC74A1 = 0x49,
    TC74A2 = 0x4A,
    TC74A3 = 0x4B,
    TC74A4 = 0x4C,
    TC74A5 = 0x4D,
    TC74A6 = 0x4E,
    TC74A7 = 0x4F,
} tc74_addr_t;

typedef struct {
    uint8_t i2c_addr;
} tc74_t;

void tc74_init(tc74_t *self, uint8_t addr);
int tc74_set_standby(tc74_t *self, bool stdby);
int tc74_read_temperature(tc74_t *self, int *temp);

#endif /* SRC_TC74_API_TC74_H_ */
