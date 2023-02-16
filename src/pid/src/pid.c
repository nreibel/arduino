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

int pid_setpoint(pid_t self, pid_type_t setpoint)
{
    self->setpoint = setpoint;
    return PID_OK;
}

int pid_process(pid_t self, pid_type_t control)
{
    const pid_type_t error = self->setpoint - control;

    const pid_type_t proportional = error;
    const pid_type_t integral = self->accumulator + error;
    const pid_type_t derivative = error - self->last_error;

    const pid_type_t output = self->bias + (self->kp * proportional) + (self->ki * integral) + (self->kd * derivative);

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
