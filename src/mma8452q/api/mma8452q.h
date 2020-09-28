#ifndef __MMA8452Q_API_H__
#define __MMA8452Q_API_H__

#include "mma8452q_prv.h"

typedef struct {
    MMA8452Q_Type_t acc_x;
    MMA8452Q_Type_t acc_y;
    MMA8452Q_Type_t acc_z;
} MMA8452Q_Data_t;

typedef enum {
    MMA8452Q_Range_2G,
    MMA8452Q_Range_4G,
    MMA8452Q_Range_8G
} MMA8452Q_Range_t;

typedef enum {
    MMA8452Q_InterruptPin_INT1,
    MMA8452Q_InterruptPin_INT2,
} MMA8452Q_InterruptPin_t;

typedef enum {
    MMA8452Q_InterruptSource_DataReady,
    MMA8452Q_InterruptSource_Freefall,
    MMA8452Q_InterruptSource_Pulse,
    MMA8452Q_InterruptSource_Orientation,
    MMA8452Q_InterruptSource_Transient,
    MMA8452Q_InterruptSource_AutoSleep
} MMA8452Q_InterruptSource_t;

Std_ReturnType MMA8452Q_Init();
void MMA8452Q_Reset();
void MMA8452Q_SetStandby(bool standby);

void MMA8452Q_GetStatus(uint8_t* status);
void MMA8452Q_GetData(MMA8452Q_Data_t* buffer);

Std_ReturnType MMA8452Q_SetRange(MMA8452Q_Range_t range, bool hpf);

// Freefall/Motion detection
void MMA8452Q_SetFreefallMode(bool evt_x, bool evt_y, bool evt_z, bool latch);
void MMA8452Q_GetFreefallSource(uint8_t* src);
void MMA8452Q_SetFreefallThreshold(float threshold);
void MMA8452Q_SetFreefallCount(uint8_t cnt);

// Transient detection
void MMA8452Q_SetTransientMode(bool evt_x, bool evt_y, bool evt_z, bool latch);
void MMA8452Q_GetTransientSource(uint8_t* src);
void MMA8452Q_SetTransientThreshold(float threshold);
void MMA8452Q_SetTransientCount(uint8_t cnt);

Std_ReturnType MMA8452Q_EnableInterrupt(MMA8452Q_InterruptSource_t interrupt);
Std_ReturnType MMA8452Q_SetInterruptConfig(MMA8452Q_InterruptSource_t src, MMA8452Q_InterruptPin_t pin);
void MMA8452Q_GetInterruptStatus(uint8_t* mask );

#endif /* __MMA8452Q_API_H__ */
