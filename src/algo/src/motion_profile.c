#include "motion_profile.h"
#include <math.h>

// Simple Profile functions

void Simple_Profile_Init(Simple_Profile_t *simple_profile, float max_change)
{
    simple_profile->max_change = max_change;
    simple_profile->target_setpoint = 0;
    simple_profile->current_setpoint = 0;
}

void Simple_Profile_Set_Max_Change(Simple_Profile_t *simple_profile, float max_change)
{
    simple_profile->max_change = max_change;
}

void Simple_Profile_Set_Target(Simple_Profile_t *simple_profile, float target_setpoint)
{
    simple_profile->target_setpoint = target_setpoint;
}

float Simple_Profile_Update(Simple_Profile_t *simple_profile)
{
    float setpoint_difference = simple_profile->target_setpoint - simple_profile->current_setpoint;

    // Limit the change in setpoint to max_change
    if (fabs(setpoint_difference) > simple_profile->max_change)
    {
        simple_profile->current_setpoint += copysign(simple_profile->max_change, setpoint_difference);
    }
    else
    {
        simple_profile->current_setpoint = simple_profile->target_setpoint;
    }

    return simple_profile->current_setpoint;
}

// Trapezoidal Profile functions

void Trapezoid_Profile_Init(Trapezoid_Profile_t *trapezoid_profile, float max_acceleration, float max_deceleration, float max_velocity)
{
    trapezoid_profile->max_acceleration = max_acceleration;
    trapezoid_profile->max_deceleration = max_deceleration;
    trapezoid_profile->max_velocity = max_velocity;
    trapezoid_profile->target_setpoint = 0;
    trapezoid_profile->current_setpoint = 0;
    trapezoid_profile->current_velocity = 0;
}

void Trapezoid_Profile_Set_Target(Trapezoid_Profile_t *trapezoid_profile, float target_setpoint)
{
    trapezoid_profile->target_setpoint = target_setpoint;
}

float Trapezoid_Profile_Update(Trapezoid_Profile_t *trapezoid_profile, float delta_time)
{
    // TODO use a timer to get delta_time !!!!

    // Calculate the distance to the target setpoint
    float distance_to_target = trapezoid_profile->target_setpoint - trapezoid_profile->current_setpoint;

    // Determine the needed acceleration based on whether we need to accelerate or decelerate
    float acceleration_needed = (trapezoid_profile->target_setpoint > trapezoid_profile->current_setpoint) ? trapezoid_profile->max_acceleration : -trapezoid_profile->max_deceleration;

    // Check if the distance to the target is greater than the stopping distance
    if (fabs(distance_to_target) > 0.5 * (trapezoid_profile->current_velocity * trapezoid_profile->current_velocity) / trapezoid_profile->max_acceleration)
    {
        // Accelerate or decelerate to reach max velocity
        trapezoid_profile->current_velocity += acceleration_needed * delta_time;

        // Cap the velocity to the maximum velocity limit
        if (fabs(trapezoid_profile->current_velocity) > trapezoid_profile->max_velocity)
        {
            trapezoid_profile->current_velocity = copysign(trapezoid_profile->max_velocity, trapezoid_profile->current_velocity);
        }
    }
    else
    {
        // Decelerate to stop at the target
        trapezoid_profile->current_velocity -= copysign(trapezoid_profile->max_deceleration, trapezoid_profile->current_velocity) * delta_time;

        // Stop the velocity if it becomes very small to prevent oscillations
        if (fabs(trapezoid_profile->current_velocity) < trapezoid_profile->max_deceleration * delta_time)
        {
            trapezoid_profile->current_velocity = 0;
        }
    }

    // Update the current setpoint based on the current velocity
    trapezoid_profile->current_setpoint += trapezoid_profile->current_velocity * delta_time;

    // Return the updated current setpoint
    return trapezoid_profile->current_setpoint;
}
