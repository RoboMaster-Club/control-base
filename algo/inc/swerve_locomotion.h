#ifndef SWERVE_LOCOMOTION_H
#define SWERVE_LOCOMOTION_H

#include "math.h"
#include "user_math.h"

#define NUMBER_OF_MODULES 4

typedef struct
{
    float speed;
    float angle;
    float last_angle;
} module_state_t;

typedef struct
{
    float v_x;
    float v_y;
    float omega;
    module_state_t states[NUMBER_OF_MODULES];
} swerve_chassis_state_t;

typedef struct
{
    float track_width;
    float wheel_base;
    float wheel_diameter;
    float max_speed;
    float max_angular_speed;
    float kinematics_matrix[8][3];
} swerve_constants_t;

swerve_constants_t swerve_init(float track_width, float wheel_base, float wheel_diameter, float max_speed, float max_angular_speed);
void calculate_swerve_kinematics(swerve_chassis_state_t *chassis_state, swerve_constants_t *swerve_constants);
void desaturate_wheel_speeds(swerve_chassis_state_t *chassis_state, swerve_constants_t *swerve_constants);
void optimize_module_angles(swerve_chassis_state_t *chassis_state, float measured_angles[NUMBER_OF_MODULES]);

#endif // SWERVE_LOCOMOTION_H
