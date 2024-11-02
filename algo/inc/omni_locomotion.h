#ifndef OMNI_LOCOMOTION_h
#define OMNI_LOCOMOTION_h

#include <math.h>

typedef struct
{
    float v_x;   // x velocity
    float v_y;   // y velocity
    float omega; // angular velocity

    float phi_dot_1; // wheel 1 angular velocity
    float phi_dot_2; // wheel 2 angular velocity
    float phi_dot_3; // wheel 3 angular velocity
    float phi_dot_4; // wheel 4 angular velocity
} omni_chassis_state_t;

typedef struct
{
    float R;         // wheel diameter [m]
    float d;         // radius from center to wheel [m]
    float theta;     // mounting angle of omni wheel [rad]
    float max_speed; // maximum speed of the robot [m/s]

    float kinematics_matrix[4][3]; // the matrix for kinematics
} omni_physical_constants_t;

omni_physical_constants_t init_omni(float R, float d, float theta, float max_speed);
void calculate_omni_kinematics(omni_chassis_state_t *input, omni_physical_constants_t *omni_physical_constants);
void desaturate_wheel_speeds(omni_chassis_state_t *input, omni_physical_constants_t *omni_physical_constants);
void convert_to_tps(omni_chassis_state_t *chassis_state);

#endif // OMNI_LOCOMOTION_h