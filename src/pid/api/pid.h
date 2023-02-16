#ifndef PID_H_
#define PID_H_

#include "types.h"
#include "pid_cfg.h"

enum {
    PID_OK,
    PID_ERR,
};

typedef struct pid_s {
    pid_type_t setpoint;

    struct {
        const pid_type_t min;
        const pid_type_t max;
    } control;

    const double kp;
    const double ki;
    const double kd;

    pid_type_t bias;
    pid_type_t last_error;
    pid_type_t accumulator;

} * pid_t;

int pid_init        (pid_t self);
int pid_setpoint    (pid_t self, pid_type_t setpoint);
int pid_process     (pid_t self, pid_type_t control);

#endif // PID_H_
