#ifndef SRC_USS_API_USS_PRV_H_
#define SRC_USS_API_USS_PRV_H_

#include "port.h"

typedef enum {
    Uninitialized,
    Ready,
    WaitForEcho,
    MeasuringEcho,
    MeasurementAvailable,
} Uss_InternalState;

/* Ticks to us ratio is set by timer prescaler */
#define TICKS_TO_US(ticks) ((ticks)/2)

/* us to cm is given by HC-SR04 spec */
#define TICKS_TO_CM(ticks) (TICKS_TO_US(ticks)/58)

/* To be defined in config file */
extern GPIO pinTrigger;
extern GPIO pinEcho;

#endif /* SRC_USS_API_USS_PRV_H_ */
