#ifndef __MMA8452Q_API_H__
#define __MMA8452Q_API_H__

typedef struct {
    uint8_t status;
    uint8_t acc_x;
    uint8_t acc_y;
    uint8_t acc_z;
} MMA8452Q_Data_t;

void MMA8452Q_Init();
void MMA8452Q_Read(MMA8452Q_Data_t* buffer);

#endif /* __MMA8452Q_API_H__ */
