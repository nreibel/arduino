#ifndef SRC_EEPROM_API_EEPROM_H_
#define SRC_EEPROM_API_EEPROM_H_

#include "types.h"
#include "eeprom_cfg.h"
#include "eeprom_prv.h"

Std_ReturnType EEPROM_BackgroundTask();

bool EEPROM_IsReady(void);

Std_ReturnType EEPROM_AsyncWrite(word ucAddress, void *ucData, int length);
Std_ReturnType EEPROM_AsyncRead (word ucAddress, void *ucData, int length);
Std_ReturnType EEPROM_SyncWrite (word ucAddress, void* ucData, int length);
Std_ReturnType EEPROM_SyncRead  (word ucAddress, void* ucData, int length);
Std_ReturnType EEPROM_MemSet    (word ucAddress, byte val, int length);

#if EEPROM_ENABLE_BLOCK_API == ON
    Std_ReturnType EEPROM_GetLastBlock(void* blk, byte type);
    Std_ReturnType EEPROM_WriteBlock(void* blk);
    Std_ReturnType EEPROM_IterateBlocks(byte type, void *buffer, Callback cbk);

    #if EEPROM_ENABLE_SERIAL_DUMP == ON
        Std_ReturnType EEPROM_DumpEEPROM(word from, word to, unsigned int len);
    #endif
#endif

#endif /* SRC_EEPROM_API_EEPROM_H_ */
