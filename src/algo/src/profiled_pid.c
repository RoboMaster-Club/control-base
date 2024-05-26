#include "profiled_pid.h"
#include <math.h>

void ProfiledPID_Init(ProfiledPID_t *profiledPID, float kp, float ki, float kd, float output_limit, float integral_limit, float dead_zone, float max_change)
{
    PID_Init(&profiledPID->pid, kp, ki, kd, output_limit, integral_limit, dead_zone);
    Profile_Init(&profiledPID->profile, max_change);
}

void ProfiledPID_Set_Target(ProfiledPID_t *profiledPID, float target_setpoint)
{
    Profile_SetSetpoint(&profiledPID->profile, target_setpoint);
}

float ProfiledPID_Update(ProfiledPID_t *profiledPID, float measurement)
{
    float current_setpoint = Profile_Update(&profiledPID->profile);
    float error = current_setpoint - measurement;

    return PID(&profiledPID->pid, error);
}
