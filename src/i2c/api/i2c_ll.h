#ifndef I2C_LL_API_H__
#define I2C_LL_API_H__

#include "types.h"

enum {
    I2C_LL_OK = 0,
    I2C_LL_FAIL,
    I2C_LL_TIMEOUT,
    I2C_LL_SLAVE_ADDRESS
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
    uint8_t twbr;
    twsr_t  twsr;
    uint8_t twar;
    uint8_t twdr;
    twcr_t  twcr;
    uint8_t twamr;
} volatile * twi_t;

#define TWI0 TYPECAST(0xB8, twi_t)

// RX callbacks
extern void i2c_ll_cbk_rx(twi_t bus, uint8_t *buffer, unsigned int length);
extern uint8_t i2c_ll_cbk_tx(twi_t bus, unsigned int offset);
extern void i2c_ll_cbk_err(twi_t bus, uint8_t status);

int i2c_ll_init_master(twi_t bus, bool fast_mode);
int i2c_ll_init_slave(twi_t twi, uint8_t addr);

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
int i2c_ll_reset_bus(twi_t twi);

#endif // I2C_LL_API_H__
