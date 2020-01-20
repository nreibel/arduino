#ifndef SRC_EEPROM_API_EEPROM_PRV_H_
#define SRC_EEPROM_API_EEPROM_PRV_H_

#include "types.h"

typedef enum {
    EEPROM_Ready,
    EEPROM_Writing,
    EEPROM_Reading,
} EepromState;

typedef struct {
    uint8_t* ramAddr;
    int      romAddr;
    int      length;
} EepromDataBuffer;

#endif /* SRC_EEPROM_API_EEPROM_PRV_H_ */
