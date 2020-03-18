#ifndef __EEPROMCFG_EEPROM_CFG_H__
#define __EEPROMCFG_EEPROM_CFG_H__

#include "eeprom_prv.h"
#include "app.h"

// EEPROM Size, in kilo Bytes
#define EEPROM_SIZE 0x400U // 1kB

// Enables a function to dump EEPROM content over Serial
// Makes use of sprintf, uses a lot of data
#define EEPROM_ENABLE_SERIAL_DUMP ON

// Enable Block API to store EEPROM data as a circular buffer of user-defined blocks
#define EEPROM_ENABLE_BLOCK_API   ON

#if EEPROM_ENABLE_BLOCK_API == ON

    typedef enum {
        BlockType_Data,
        BlockType_DeviceInfo,
    } BlockType;

    typedef struct {
        Header   header;
        byte     device_id[16];
        uint32_t unlock_crc32;
    } DeviceInfoBlock;

    typedef struct {
        Header header;
        byte   nb_alerts[SENSORS_COUNT];
        byte   unlock_attempts;
    } DataBlock;

#endif

#endif // __EEPROMCFG_EEPROM_CFG_H__
