#ifndef __PMBUS_API_H__
#define __PMBUS_API_H__

#include "types.h"
#include "i2c.h"

enum {
    PMBUS_OK,
    PMBUS_ERROR,
    PMBUS_NOT_SUPPORTED,
    PMBUS_ERROR_IO,
};

enum {
    PMBUS_PAGE = 0x00,
    PMBUS_OPERATION,
    /* ... */
    PMBUS_CAPABILITY = 0x19,
    PMBUS_QUERY,
    /* ... */
    PMBUS_VOUT_MODE = 0x20,
    PMBUS_VOUT_COMMAND,
    /* ... */
    PMBUS_VOUT_OV_FAULT_LIMIT = 0x40,
    PMBUS_VOUT_OV_FAULT_RESPONSE,
    PMBUS_VOUT_OV_WARN_LIMIT,
    PMBUS_VOUT_UV_WARN_LIMIT,
    PMBUS_VOUT_UV_FAULT_LIMIT,
    PMBUS_VOUT_UV_FAULT_RESPONSE,
    PMBUS_IOUT_OC_FAULT_LIMIT,
    PMBUS_IOUT_OC_FAULT_RESPONSE,
    PMBUS_IOUT_OC_LV_FAULT_LIMIT,
    PMBUS_IOUT_OC_LV_FAULT_RESPONSE,
    PMBUS_IOUT_OC_WARN_LIMIT,
    PMBUS_IOUT_UC_FAULT_LIMIT,
    PMBUS_IOUT_UC_FAULT_RESPONSE,
    /* ... */
    PMBUS_OT_FAULT_LIMIT = 0x4F,
    PMBUS_OT_FAULT_RESPONSE,
    PMBUS_OT_WARN_LIMIT,
    PMBUS_UT_WARN_LIMIT,
    PMBUS_UT_FAULT_LIMIT,
    PMBUS_UT_FAULT_RESPONSE,
    PMBUS_VIN_OV_FAULT_LIMIT,
    PMBUS_VIN_OV_FAULT_RESPONSE,
    PMBUS_VIN_OV_WARN_LIMIT,
    PMBUS_VIN_UV_WARN_LIMIT,
    PMBUS_VIN_UV_FAULT_LIMIT,
    PMBUS_VIN_UV_FAULT_RESPONSE,
    PMBUS_IIN_OC_FAULT_LIMIT,
    PMBUS_IIN_OC_FAULT_RESPONSE,
    PMBUS_IIN_OC_WARN_LIMIT,
    PMBUS_POWER_GOOD_ON,
    PMBUS_POWER_GOOD_OFF,
    PMBUS_TON_DELAY,
    PMBUS_TON_RISE,
    PMBUS_TON_MAX_FAULT_LIMIT,
    PMBUS_TON_MAX_FAULT_RESPONSE,
    PMBUS_TOFF_DELAY,
    PMBUS_TOFF_FALL,
    PMBUS_TOFF_MAX_WARN_LIMIT,
    /* ... */
    PMBUS_POUT_OP_FAULT_LIMIT = 0x68,
    PMBUS_POUT_OP_FAULT_RESPONSE,
    PMBUS_POUT_OP_WARN_LIMIT,
    PMBUS_PIN_OP_WARN_LIMIT,
    /* ... */
    PMBUS_STATUS_BYTE = 0x78,
    PMBUS_STATUS_WORD,
    PMBUS_STATUS_VOUT,
    PMBUS_STATUS_IOUT,
    PMBUS_STATUS_INPUT,
    PMBUS_STATUS_TEMPERATURE,
    PMBUS_STATUS_CML,
    PMBUS_STATUS_OTHER,
    PMBUS_STATUS_MFR_SPECIFIC,
    PMBUS_STATUS_FANS_1_2,
    PMBUS_STATUS_FANS_3_4,
    /* ... */
    PMBUS_READ_VIN = 0x88,
    PMBUS_READ_IIN,
    PMBUS_READ_VCAP,
    PMBUS_READ_VOUT,
    PMBUS_READ_IOUT,
    PMBUS_READ_TEMPERATURE_1,
    PMBUS_READ_TEMPERATURE_2,
    PMBUS_READ_TEMPERATURE_3,
    PMBUS_READ_FAN_SPEED_1,
    PMBUS_READ_FAN_SPEED_2,
    PMBUS_READ_FAN_SPEED_3,
    PMBUS_READ_FAN_SPEED_4,
    PMBUS_READ_DUTY_CYCLE,
    PMBUS_READ_FREQUENCY,
    PMBUS_READ_POUT,
    PMBUS_READ_PIN,
    PMBUS_PMBUS_REVISION,
    PMBUS_MFR_ID,
    PMBUS_MFR_MODEL,
    PMBUS_MFR_REVISION,
    PMBUS_MFR_LOCATION,
    PMBUS_MFR_DATE,
    PMBUS_MFR_SERIAL,
    /* ... */
    PMBUS_MFR_VIN_MIN = 0xA0,
    PMBUS_MFR_VIN_MAX,
    PMBUS_MFR_IIN_MAX,
    PMBUS_MFR_PIN_MAX,
    PMBUS_MFR_VOUT_MIN,
    PMBUS_MFR_VOUT_MAX,
    PMBUS_MFR_IOUT_MAX,
    PMBUS_MFR_POUT_MAX,
    PMBUS_MFR_TAMBIENT_MAX,
    PMBUS_MFR_TAMBIENT_MIN,
    PMBUS_MFR_EFFICIENCY_LL,
    PMBUS_MFR_EFFICIENCY_HL,
    /* ... */
    PMBUS_MFR_BLACKBOX = 0xDC,
    PMBUS_MFR_REAL_TIME_BLACK_BOX,
    PMBUS_MFR_SYSTEM_BLACK_BOX,
    PMBUS_MFR_BLACKBOX_CONFIG
};

typedef union {
    struct {
        uint8_t p : 5;
        uint8_t m : 3;
    } decoded;
    uint8_t raw;
} vout_mode_t;

typedef union {
    struct {
        uint16_t mantissa : 11;
        uint16_t exponent : 5;
    } decoded;
    uint16_t raw;
} linear11_t;

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

#if OS_MALLOC
pmbus_t pmbus_create(i2c_bus_t bus, uint8_t addr);
void pmbus_destroy(pmbus_t self);
#endif

// Object methods
int pmbus_read_mfr_model   (pmbus_t self, char *buffer, unsigned int len);
int pmbus_read_temperature (pmbus_t self, double *temperature);
int pmbus_read_fanspeed    (pmbus_t self, unsigned int *fanspeed);
int pmbus_read_vin         (pmbus_t self, double *vin);
int pmbus_read_iin         (pmbus_t self, double *iin);
int pmbus_read_pin         (pmbus_t self, double *pin);
int pmbus_read_vout        (pmbus_t self, double *vout);
int pmbus_read_iout        (pmbus_t self, double *iout);
int pmbus_read_pout        (pmbus_t self, double *pout);
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
uint16_t pmbus_linear11_encode(double value);

// Manufacturer specific
int pmbus_gw_read_page(pmbus_t self, unsigned int *page);
int pmbus_gw_write_page(pmbus_t self, const unsigned int page);
int pmbus_gw_read_pos_total(pmbus_t self, long int *pos);
int pmbus_gw_read_pos_last(pmbus_t self, long int *pos);

#endif /* __PMBUS_API_H__ */
