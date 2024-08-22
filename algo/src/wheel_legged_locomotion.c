#include "wheel_legged_locomotion.h"

Wheel_Leg_t wheel_leg[2];

void Wheel_Legged_Locomotion_Init()
{
    wheel_leg[0].state_vector[0] = 0.0f;
    wheel_leg[0].state_vector[1] = 0.0f;
    wheel_leg[0].state_vector[2] = 0.0f;
    wheel_leg[0].state_vector[3] = 0.0f;
    wheel_leg[0].state_vector[4] = 0.0f;
    wheel_leg[0].state_vector[5] = 0.0f;

    wheel_leg[1].state_vector[0] = 0.0f;
    wheel_leg[1].state_vector[1] = 0.0f;
    wheel_leg[1].state_vector[1] = 0.0f;
    wheel_leg[1].state_vector[2] = 0.0f;
    wheel_leg[1].state_vector[3] = 0.0f;
    wheel_leg[1].state_vector[4] = 0.0f;
    wheel_leg[1].state_vector[5] = 0.0f;
}

void Wheel_Legged_Update_State()
{
    /* Forward Kinematics */
}

void Wheel_Legged_Locomotion()
{
    /* Linear Quadratic Regulator */

    /* Torque Mapping */
    
    /* Output */
}