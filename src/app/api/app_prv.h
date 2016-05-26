#ifndef SRC_APP_API_APP_PRV_H_
#define SRC_APP_API_APP_PRV_H_

#include "port.h"
#include "app_cfg.h"

extern PinDef Pin_LED_Chords;
extern PinDef Pin_LED_Classic;

typedef struct {
	WhammyMode whammyMode;
	boolean    chordsMode;
} EEPROM_Data;

void Task_MainCyclic(void);

void Whammy_ProgramChange();

#endif /* SRC_APP_API_APP_PRV_H_ */
