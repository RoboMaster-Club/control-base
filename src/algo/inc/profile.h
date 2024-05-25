#ifndef PROFILE_H
#define PROFILE_H

typedef struct
{
    float max_change;       // Maximum change in setpoint per update
    float target_setpoint;  // The desired setpoint
    float current_setpoint; // The current setpoint being used
} Profile_t;

void Profile_Init(Profile_t *profile, float max_change);
void Profile_Set_Target(Profile_t *profile, float target_setpoint);
float Profile_Update(Profile_t *profile);

#endif // PROFILE_H
