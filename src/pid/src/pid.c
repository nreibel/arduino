#include "pid.h"
#include <string.h>

int pid_init(pid_t self)
{
    self->setpoint = 0;
    self->bias = 0;
    self->last_error = 0;
    self->accumulator = 0;
    return PID_OK;
}

int pid_setpoint(pid_t self, int setpoint)
{
    self->setpoint = setpoint;
    return PID_OK;
}

int pid_process(pid_t self, int control)
{
    const int error = self->setpoint - control;

    const int proportional = error;
    const int integral = self->accumulator + error;
    const int derivative = error - self->last_error;

    const int output = self->bias + (self->kp * proportional) + (self->ki * integral) + (self->kd * derivative);

    self->last_error = error;

    // Output clamping + integral windup
    if (output <= self->control.min)
    {
        if (self->kp * error > 0)
            self->accumulator = integral;

        return self->control.min;
    }
    else if (output >= self->control.max)
    {
        if (self->kp * error < 0)
            self->accumulator = integral;

        return self->control.max;
    }
    else
    {
        self->accumulator = integral;
        return output;
    }
}
