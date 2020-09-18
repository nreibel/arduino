#ifndef SRC_EEPROM_API_EEPROM_H_
#define SRC_EEPROM_API_EEPROM_H_

#include "types.h"
#include "eeprom_cfg.h"
#include "eeprom_prv.h"

Std_ReturnType EEPROM_BackgroundTask();

bool EEPROM_IsReady(void);

Std_ReturnType EEPROM_AsyncWrite(uint16_t ucAddress, void *ucData, int length);
Std_ReturnType EEPROM_AsyncRead (uint16_t ucAddress, void *ucData, int length);
Std_ReturnType EEPROM_SyncWrite (uint16_t ucAddress, void* ucData, int length);
Std_ReturnType EEPROM_SyncRead  (uint16_t ucAddress, void* ucData, int length);
Std_ReturnType EEPROM_MemSet    (uint16_t ucAddress, uint8_t val, int length);

#if EEPROM_ENABLE_SERIAL_DUMP == ON
    Std_ReturnType EEPROM_DumpEEPROM_Raw(uint16_t from, uint16_t to);
    Std_ReturnType EEPROM_DumpEEPROM(uint16_t from, uint16_t to, int line_length);
#endif

#if EEPROM_ENABLE_BLOCK_API == ON
    Std_ReturnType EEPROM_GetLastBlock(void* blk, uint8_t type);
    Std_ReturnType EEPROM_WriteBlock(void* blk);
    Std_ReturnType EEPROM_IterateBlocks(uint8_t type, void *buffer, Callback cbk);
#endif

#endif /* SRC_EEPROM_API_EEPROM_H_ */
