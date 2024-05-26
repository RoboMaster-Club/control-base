#ifndef MOTION_PROFILE_H
#define MOTION_PROFILE_H

// Simple Profile
typedef struct
{
    float max_change;       // Maximum change in setpoint per update
    float target_setpoint;  // The desired setpoint
    float current_setpoint; // The current setpoint being used
} Simple_Profile_t;

void Simple_Profile_Init(Simple_Profile_t *simple_profile, float max_change);
void Simple_Profile_Set_Target(Simple_Profile_t *simple_profile, float target_setpoint);
float Simple_Profile_Update(Simple_Profile_t *simple_profile);

// Trapezoid Profile
typedef struct
{
    float max_acceleration; // Maximum acceleration
    float max_deceleration; // Maximum deceleration
    float max_velocity;     // Maximum velocity
    float target_setpoint;  // The desired setpoint
    float current_setpoint; // The current setpoint being used
    float current_velocity; // The current velocity
} Trapezoid_Profile_t;

void Trapezoid_Profile_Init(Trapezoid_Profile_t *trapezoid_profile, float max_acceleration, float max_deceleration, float max_velocity);
void Trapezoid_Profile_Set_Target(Trapezoid_Profile_t *trapezoid_profile, float target_setpoint);
float Trapezoid_Profile_Update(Trapezoid_Profile_t *trapezoid_profile, float delta_time);

#endif // MOTION_PROFILE_H
