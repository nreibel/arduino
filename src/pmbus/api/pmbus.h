#ifndef __PMBUS_API_H__
#define __PMBUS_API_H__

#include "types.h"
#include "i2c.h"

#define PMBUS_MFR_MODEL_MAX_LENGTH 0x10

typedef struct pmbus_prv_s {
    struct i2c_device_prv_s dev;
} * pmbus_t;

struct blackbox_event_data_s {
    uint32_t psu_on_time : 24;
    uint32_t rtc_data; // Reserved for future use
    uint16_t number_of_ac_power_cycles;
    uint16_t number_of_pson_power_cycles;
    uint16_t STATUS_WORD;
    uint8_t STATUS_IOUT;
    uint8_t STATUS_INPUT;
    uint8_t STATUS_TEMPERATURE;
    uint8_t STATUS_FAN_1_2;
    uint16_t READ_VIN;
    uint16_t READ_IIN;
    uint16_t READ_IOUT;
    uint16_t READ_TEMPERATURE_1;
    uint16_t READ_TEMPERATURE_2;
    uint16_t READ_FAN_SPEED_1;
    uint16_t READ_PIN;
    uint16_t READ_VOUT;
    uint8_t under_voltage_on_input : 4;
    uint8_t thermal_shutdown : 4;
    uint8_t over_current_on_output : 4;
    uint8_t general_failure_shutdown : 4;
    uint8_t fan_failure_shutdown : 4;
    uint8_t over_voltage_on_output : 4;
    uint8_t input_voltage_warning : 4;
    uint8_t thermal_warning : 4;
    uint8_t output_current_warning : 4;
    uint8_t fan_slow_warning : 4;
} __attribute__((packed));

struct blackbox_data_s {
    uint8_t length;
    uint8_t system_top_assembly_number[10];
    uint8_t system_serial_number[10];
    uint8_t motherboard_assembly_number[10];
    uint8_t motherboard_serial_number[10];
    uint32_t psu_on_time : 24;
    uint16_t number_of_ac_power_cycles;
    uint16_t number_of_pson_power_cycles;
    struct blackbox_event_data_s events[5];
} __attribute__((packed));

int pmbus_init(pmbus_t self, i2c_bus_t bus, uint8_t addr);
int pmbus_read_mfr_model(pmbus_t self, char *buffer);
int pmbus_read_temperature(pmbus_t self, double *temperature);
int pmbus_read_fanspeed(pmbus_t self, unsigned int *fanspeed);
int pmbus_read_vin(pmbus_t self, double *vin);
int pmbus_read_iin(pmbus_t self, double *vin);
int pmbus_read_pin(pmbus_t self, double *vin);
int pmbus_read_vout(pmbus_t self, double *vout);
int pmbus_read_iout(pmbus_t self, double *vout);
int pmbus_read_blackbox(pmbus_t self, struct blackbox_data_s * data);
int pmbus_vout_decode(pmbus_t self, uint16_t raw, double *value);

int pmbus_read_status_word(pmbus_t self, uint16_t *value);
int pmbus_read_status_vout(pmbus_t self, uint16_t *value);
int pmbus_read_status_iout(pmbus_t self, uint8_t *value);
int pmbus_read_status_input(pmbus_t self, uint8_t *value);
int pmbus_read_status_temperature(pmbus_t self, uint8_t *value);
int pmbus_read_status_cml(pmbus_t self, uint8_t *value);
int pmbus_read_status_fans_1_2(pmbus_t self, uint8_t *value);

double pmbus_linear11_decode(uint16_t data);

// Manufacturer specific

int pmbus_gw_read_page(pmbus_t self, unsigned int *page);
int pmbus_gw_write_page(pmbus_t self, const unsigned int page);
int pmbus_gw_read_pos_total(pmbus_t self, long int *pos);
int pmbus_gw_read_pos_last(pmbus_t self, long int *pos);

#endif /* __PMBUS_API_H__ */
