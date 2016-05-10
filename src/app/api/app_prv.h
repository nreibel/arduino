#ifndef SRC_APP_API_APP_PRV_H_
#define SRC_APP_API_APP_PRV_H_

#include "port.h"
#include "app_cfg.h"

extern PinDef Pin_LED;
extern PinDef Pin_BtnUp;
extern PinDef Pin_BtnDown;
extern PinDef Pin_Preset;

void Task_MainCyclic(void);

void Whammy_ProgramChange(WhammyMode newProgram, boolean chordsMode, boolean bypass);

#endif /* SRC_APP_API_APP_PRV_H_ */
