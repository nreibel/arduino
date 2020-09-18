#ifndef SRC_EEPROM_API_EEPROM_PRV_H_
#define SRC_EEPROM_API_EEPROM_PRV_H_

#include "types.h"

typedef enum {
    EEPROM_Ready,
    EEPROM_Writing,
    EEPROM_Reading,
} EepromState;

typedef struct {
    void    *ramAddr;
    uint16_t romAddr;
    int      length;
} EepromDataBuffer;

typedef struct {
    uint16_t id;
    uint8_t  type;
    uint8_t  size;
} Header;

#endif /* SRC_EEPROM_API_EEPROM_PRV_H_ */
