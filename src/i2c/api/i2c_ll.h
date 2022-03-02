#ifndef I2C_LL_API_H__
#define I2C_LL_API_H__

#include "types.h"

enum {
    I2C_LL_OK = 0,
    I2C_LL_FAIL,
    I2C_LL_TIMEOUT
};

enum {
    I2C_LL_PSCL_1  = 0x0,
    I2C_LL_PSCL_4  = 0x1,
    I2C_LL_PSCL_16 = 0x2,
    I2C_LL_PSCL_64 = 0x3,
};

typedef union {
    struct {
        uint8_t twie       : 1;
        uint8_t __reserved : 1;
        uint8_t twen       : 1;
        uint8_t twwc       : 1;
        uint8_t twsto      : 1;
        uint8_t twsta      : 1;
        uint8_t twea       : 1;
        uint8_t twint      : 1;
    } bits;
    uint8_t reg;
} twcr_t;

typedef union {
    struct {
        uint8_t twps       : 2;
        uint8_t __reserved : 1;
        uint8_t tws        : 5;
    } bits;
    uint8_t reg;
} twsr_t;

typedef struct {
    volatile uint8_t twbr;
    volatile twsr_t  twsr;
    volatile uint8_t twar;
    volatile uint8_t twdr;
    volatile twcr_t  twcr;
    volatile uint8_t twamr;
} * twi_t;

#define TWI0 __MEM(0xB8, const twi_t)

int i2c_ll_init(twi_t bus, bool fast_mode);
int i2c_ll_wait_int(twi_t bus, unsigned int ms);
int i2c_ll_wait_stop(twi_t bus, unsigned int ms);
int i2c_ll_start_condition(twi_t bus);
int i2c_ll_restart_condition(twi_t bus);
int i2c_ll_stop_condition(twi_t bus);
int i2c_ll_slave_write(twi_t bus, uint8_t addr);
int i2c_ll_slave_read(twi_t bus, uint8_t addr);
int i2c_ll_write(twi_t bus, uint8_t data);
int i2c_ll_read_ack(twi_t bus, uint8_t *data);
int i2c_ll_read_nack(twi_t bus, uint8_t *data);

#endif // I2C_LL_API_H__
