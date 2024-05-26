#ifndef PROFILED_PID_H
#define PROFILED_PID_H

#include "pid.h"
#include "motion_profile.h"

typedef struct
{
    PID_t pid;
    Motion_Profile_t profile;
} ProfiledPID_t;

void ProfiledPID_Init(ProfiledPID_t *profiledPID, float kp, float ki, float kd, float output_limit, float integral_limit, float dead_zone, float max_change);
void ProfiledPID_Set_Target(ProfiledPID_t *profiledPID, float target);
float ProfiledPID_Update(ProfiledPID_t *profiledPID, float measurement);

#endif // PROFILED_PID_H
