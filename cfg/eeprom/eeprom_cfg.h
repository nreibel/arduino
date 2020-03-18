#ifndef __EEPROMCFG_EEPROM_CFG_H__
#define __EEPROMCFG_EEPROM_CFG_H__

#include "eeprom_prv.h"

// EEPROM Size, in kilo Bytes
#define EEPROM_SIZE 0x400U // 1kB

// Enable Block API to store EEPROM data as a circular buffer of user-defined blocks
#define EEPROM_ENABLE_BLOCK_API   ON

#if EEPROM_ENABLE_BLOCK_API == ON

    #define EEPROM_ENABLE_SERIAL_DUMP ON

    typedef struct {
        Header   header;
        byte     device_id[16];
        uint32_t unlock_crc32;
    } DeviceInfoBlock;

#endif

#endif // __EEPROMCFG_EEPROM_CFG_H__
