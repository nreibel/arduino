#ifndef I2C_LL_API_H__
#define I2C_LL_API_H__

#include "types.h"

enum {
    I2C_LL_OK = 0,
    I2C_LL_ERR_PARAMETER,
    I2C_LL_ERR_STATUS,
    I2C_LL_ERR_TIMEOUT,
};

typedef enum {
    I2C_EVENT_RX_START,
    I2C_EVENT_TX_START,
    I2C_EVENT_RX_MORE,
    I2C_EVENT_TX_MORE,
    I2C_EVENT_RX_COMPLETE,
    I2C_EVENT_TX_COMPLETE,
    I2C_EVENT_SEQ_ERROR,
    I2C_EVENT_BUS_ERROR,
} i2c_event_t;

typedef enum {
    I2C_LL_100KHZ,
    I2C_LL_400KHZ,
} i2c_ll_clk_t;

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

// I2C Slave Callback
extern void i2c_ll_callback(twi_t twi, i2c_event_t event, unsigned int size);

// I2C Slave RX/TX Buffers
int i2c_ll_set_rx_buffer(twi_t twi, void * buffer, unsigned int len);
int i2c_ll_set_tx_buffer(twi_t twi, const void * buffer, unsigned int len);

int i2c_ll_init_master(twi_t bus, i2c_ll_clk_t clk);
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
