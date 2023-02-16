#ifndef PID_H_
#define PID_H_

#include "types.h"

enum {
    PID_OK,
    PID_ERR,
};

typedef struct pid_s {
    int setpoint;

    struct {
        const int min;
        const int max;
    } control;

    const double kp;
    const double ki;
    const double kd;

    int bias;
    int last_error;
    int accumulator;

} * pid_t;

int pid_init        (pid_t self);
int pid_setpoint    (pid_t self, int setpoint);
int pid_process     (pid_t self, int control);

#endif // PID_H_
