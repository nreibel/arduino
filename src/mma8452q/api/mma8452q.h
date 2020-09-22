#ifndef __MMA8452Q_API_H__
#define __MMA8452Q_API_H__

// TODO : move to config
#define READ_MODE_FAST OFF

#if READ_MODE_FAST == ON
typedef uint8_t MMA8452Q_Type_t;
#else
typedef int16_t MMA8452Q_Type_t;
#endif

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

// Interrupt bits
#define MMA8452Q_INTERRUPT_DATAREADY   0
#define MMA8452Q_INTERRUPT_FREEFALL    2
#define MMA8452Q_INTERRUPT_PULSE       3
#define MMA8452Q_INTERRUPT_ORIENTATION 4
#define MMA8452Q_INTERRUPT_TRANSIENT   5
#define MMA8452Q_INTERRUPT_AUTOSLEEP   7

Std_ReturnType MMA8452Q_Init();
void MMA8452Q_Reset();
void MMA8452Q_SetStandby(bool standby);

void MMA8452Q_GetStatus(uint8_t* status);
void MMA8452Q_GetData(MMA8452Q_Data_t* buffer);

Std_ReturnType MMA8452Q_SetRange(MMA8452Q_Range_t range, bool hpf);

void MMA8452Q_SetTransientCount(uint8_t cnt);
void MMA8452Q_SetTransientMode(bool x, bool y, bool z, bool latch);
void MMA8452Q_SetTransientThreshold(float threshold);
void MMA8452Q_GetTransientSource(uint8_t* src);

void MMA8452Q_EnableInterrupts(uint8_t mask);
void MMA8452Q_SetInterruptsConfig(uint8_t config);
void MMA8452Q_GetInterruptStatus(uint8_t* status);

#endif /* __MMA8452Q_API_H__ */
