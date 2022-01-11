#ifndef __PMBUS_API_H__
#define __PMBUS_API_H__

#include "types.h"
#include "i2c.h"

#define PMBUS_MFR_MODEL_MAX_LENGTH 0x10

typedef enum
{
    PMBUS_OK,
    PMBUS_ERROR,
    PMBUS_NOT_SUPPORTED,
    PMBUS_ERROR_IO,
} pmbus_error_t;

typedef struct pmbus_prv_s {
    struct i2c_device_prv_s dev;
    uint8_t vout_mode;
} * pmbus_t;

typedef union
{
    struct
    {
        uint16_t none_of_the_above : 1;
        uint16_t cml : 1;
        uint16_t temperature : 1;
        uint16_t vin_uv : 1;
        uint16_t iout_oc : 1;
        uint16_t vout_ov : 1;
        uint16_t off : 1;
        uint16_t busy : 1;
        uint16_t unknown : 1;
        uint16_t other : 1;
        uint16_t fans : 1;
        uint16_t power_good_negated : 1;
        uint16_t mfr : 1;
        uint16_t input : 1;
        uint16_t iout_pout : 1;
        uint16_t vout : 1;
    } decoded;
    uint16_t raw;
} pmbus_status_word_t;

typedef union
{
    struct
    {
        uint8_t vout_tracking_error : 1;
        uint8_t toff_max_warning : 1;
        uint8_t ton_max_fault : 1;
        uint8_t vout_max_warning : 1;
        uint8_t vout_uv_fault : 1;
        uint8_t vout_uv_warning : 1;
        uint8_t vout_ov_warning : 1;
        uint8_t vout_ov_fault : 1;
    } decoded;
    uint8_t raw;
} pmbus_status_vout_t;

typedef union
{
    struct
    {
        uint8_t pout_op_warning : 1;
        uint8_t pout_op_fault : 1;
        uint8_t power_limiting : 1;
        uint8_t current_share_fault : 1;
        uint8_t iout_uc_fault : 1;
        uint8_t iout_oc_warning : 1;
        uint8_t iout_oc_lv_shutdown_fault : 1;
        uint8_t iout_oc_fault : 1;
    } decoded;
    uint8_t raw;
} pmbus_status_iout_t;

typedef union
{
    struct
    {
        uint8_t pin_op_warning : 1;
        uint8_t iin_oc_warning : 1;
        uint8_t iin_oc_fault : 1;
        uint8_t insufficient_input_voltage : 1;
        uint8_t vin_uv_fault : 1;
        uint8_t vin_uv_warning : 1;
        uint8_t vin_ov_warning : 1;
        uint8_t vin_ov_fault : 1;
    } decoded;
    uint8_t raw;
} pmbus_status_input_t;

typedef union
{
    struct
    {
        uint8_t reserved : 4;
        uint8_t undertemperature_fault : 1;
        uint8_t undertemperature_warning : 1;
        uint8_t overtemperature_warning : 1;
        uint8_t overtemperature_fault : 1;
    } decoded;
    uint8_t raw;
} pmbus_status_temperature_t;

typedef union
{
    struct
    {
        uint8_t other_mem_log_fault : 1;
        uint8_t other_comm_fault : 1;
        uint8_t reserved : 1;
        uint8_t processor_fault : 1;
        uint8_t memory_fault : 1;
        uint8_t packet_error_check : 1;
        uint8_t invalid_or_unsupported_data : 1;
        uint8_t invalid_or_unsupported_command : 1;
    } decoded;
    uint8_t raw;
} pmbus_status_cml_t;

typedef union
{
    struct
    {
        uint8_t airflow_warning : 1;
        uint8_t airflow_fault : 1;
        uint8_t fan_2_speed_overridden : 1;
        uint8_t fan_1_speed_overridden : 1;
        uint8_t fan_2_warning : 1;
        uint8_t fan_1_warning : 1;
        uint8_t fan_2_fault : 1;
        uint8_t fan_1_fault : 1;
    } decoded;
    uint8_t raw;
} pmbus_status_fans_t;

typedef union
{
    struct __attribute__((packed))
    {
        uint32_t psu_on_time : 24;
        uint32_t rtc_data; // Reserved for future use
        uint16_t number_of_ac_power_cycles;
        uint16_t number_of_pson_power_cycles;
        pmbus_status_word_t STATUS_WORD;
        pmbus_status_iout_t STATUS_IOUT;
        pmbus_status_input_t STATUS_INPUT;
        pmbus_status_temperature_t STATUS_TEMPERATURE;
        pmbus_status_fans_t STATUS_FAN_1_2;
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
    } decoded;
    uint8_t *raw;
} pmbus_blackbox_event_t;

typedef union
{
    struct __attribute__((packed))
    {
        uint8_t length;
        uint8_t system_top_assembly_number[10];
        uint8_t system_serial_number[10];
        uint8_t motherboard_assembly_number[10];
        uint8_t motherboard_serial_number[10];
        uint32_t psu_on_time : 24;
        uint16_t number_of_ac_power_cycles;
        uint16_t number_of_pson_power_cycles;
        pmbus_blackbox_event_t events[5];
    } decoded;
    uint8_t *raw;
} pmbus_blackbox_t;

// Initializers and constructors
int pmbus_init(pmbus_t self, i2c_bus_t bus, uint8_t addr);

// Object methods
int pmbus_read_mfr_model   (pmbus_t self, char *buffer);
int pmbus_read_temperature (pmbus_t self, double *temperature);
int pmbus_read_fanspeed    (pmbus_t self, unsigned int *fanspeed);
int pmbus_read_vin         (pmbus_t self, double *vin);
int pmbus_read_iin         (pmbus_t self, double *vin);
int pmbus_read_pin         (pmbus_t self, double *vin);
int pmbus_read_vout        (pmbus_t self, double *vout);
int pmbus_read_iout        (pmbus_t self, double *vout);
int pmbus_read_blackbox    (pmbus_t self, pmbus_blackbox_t *data);
int pmbus_vout_decode      (pmbus_t self, uint16_t raw, double *value);

// Read status bytes
int pmbus_read_status_word        (pmbus_t self, pmbus_status_word_t *value);
int pmbus_read_status_vout        (pmbus_t self, pmbus_status_vout_t *value);
int pmbus_read_status_iout        (pmbus_t self, pmbus_status_iout_t *value);
int pmbus_read_status_input       (pmbus_t self, pmbus_status_input_t *value);
int pmbus_read_status_temperature (pmbus_t self, pmbus_status_temperature_t *value);
int pmbus_read_status_cml         (pmbus_t self, pmbus_status_cml_t *value);
int pmbus_read_status_fans_1_2    (pmbus_t self, pmbus_status_fans_t *value);

// Class methods
double pmbus_linear11_decode(uint16_t data);

// Manufacturer specific
int pmbus_gw_read_page(pmbus_t self, unsigned int *page);
int pmbus_gw_write_page(pmbus_t self, const unsigned int page);
int pmbus_gw_read_pos_total(pmbus_t self, long int *pos);
int pmbus_gw_read_pos_last(pmbus_t self, long int *pos);

#endif /* __PMBUS_API_H__ */
