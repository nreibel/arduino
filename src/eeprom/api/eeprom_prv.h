#ifndef SRC_EEPROM_API_EEPROM_PRV_H_
#define SRC_EEPROM_API_EEPROM_PRV_H_

#include "types.h"

typedef enum {
    EEPROM_Ready,
    EEPROM_Writing,
    EEPROM_Reading,
} EepromState;

typedef struct {
    void *ramAddr;
    word  romAddr;
    int   length;
} EepromDataBuffer;

typedef struct {
    word id;
    byte type;
    byte size;
} Header;

#endif /* SRC_EEPROM_API_EEPROM_PRV_H_ */
