#include "profile.h"
#include <math.h>

void Profile_Init(Profile_t *profile, float max_change)
{
    profile->max_change = max_change;
    profile->target_setpoint = 0;
    profile->current_setpoint = 0;
}

void Profile_Set_Target(Profile_t *profile, float target_setpoint)
{
    profile->target_setpoint = target_setpoint;
}

float Profile_Update(Profile_t *profile)
{
    float setpoint_difference = profile->target_setpoint - profile->current_setpoint;

    // Limit the change in setpoint to max_change
    if (fabs(setpoint_difference) > profile->max_change)
    {
        profile->current_setpoint += copysign(profile->max_change, setpoint_difference);
    }
    else
    {
        profile->current_setpoint = profile->target_setpoint;
    }

    return profile->current_setpoint;
}
