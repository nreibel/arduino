#ifndef SRC_EEPROM_API_EEPROM_H_
#define SRC_EEPROM_API_EEPROM_H_

#include "types.h"

void EEPROM_BackgroundTask();

boolean EEPROM_IsReady(void);

Std_ReturnType EEPROM_AsyncWrite(int ucAddress, uint8_t *ucData, int length);
Std_ReturnType EEPROM_AsyncRead (int ucAddress, uint8_t *ucData, int length);
Std_ReturnType EEPROM_SyncWrite (int ucAddress, uint8_t* ucData, int length);
Std_ReturnType EEPROM_SyncRead  (int ucAddress, uint8_t* ucData, int length);


#endif /* SRC_EEPROM_API_EEPROM_H_ */
