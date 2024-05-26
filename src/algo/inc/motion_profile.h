#ifndef MOTION_PROFILE_H
#define MOTION_PROFILE_H

typedef struct
{
    float max_change;       // Maximum change in setpoint per update
    float target_setpoint;  // The desired setpoint
    float current_setpoint; // The current setpoint being used
} Motion_Profile_t;

void Motion_Profile_Init(Motion_Profile_t *motion_profile, float max_change);
void Motion_Profile_Set_Target(Motion_Profile_t *motion_profile, float target_setpoint);
float Motion_Profile_Update(Motion_Profile_t *motion_profile);

#endif // MOTION_PROFILE_H
