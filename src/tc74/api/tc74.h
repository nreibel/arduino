#ifndef SRC_TC74_API_TC74_H_
#define SRC_TC74_API_TC74_H_

#include "types.h"

typedef struct {
    uint8_t i2c_addr;
} tc74_t;

void tc74_init(tc74_t *self, uint8_t addr);
void tc74_set_standby(tc74_t *self, bool stdby);
void tc74_read_temperature(tc74_t *self, int *temp);

Std_ReturnType TC74_Init();
Std_ReturnType TC74_SetStandby(uint8_t slave, bool stdby);
Std_ReturnType TC74_Read(uint8_t slave, uint8_t *temp);

#endif /* SRC_TC74_API_TC74_H_ */
