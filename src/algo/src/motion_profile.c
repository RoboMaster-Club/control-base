#include "motion_profile.h"
#include <math.h>

void Motion_Profile_Init(Motion_Profile_t *motion_profile, float max_change)
{
    motion_profile->max_change = max_change;
    motion_profile->target_setpoint = 0;
    motion_profile->current_setpoint = 0;
}

void Motion_Profile_Set_Max_Change(Motion_Profile_t *motion_profile, float max_change)
{
    motion_profile->max_change = max_change;
}

void Motion_Profile_Set_Target(Motion_Profile_t *motion_profile, float target_setpoint)
{
    motion_profile->target_setpoint = target_setpoint;
}

float Motion_Profile_Update(Motion_Profile_t *motion_profile)
{
    float setpoint_difference = motion_profile->target_setpoint - motion_profile->current_setpoint;

    // Limit the change in setpoint to max_change
    if (fabs(setpoint_difference) > motion_profile->max_change)
    {
        motion_profile->current_setpoint += copysign(motion_profile->max_change, setpoint_difference);
    }
    else
    {
        motion_profile->current_setpoint = motion_profile->target_setpoint;
    }

    return motion_profile->current_setpoint;
}
