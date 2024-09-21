#include "omni_locomotion.h"

/**
 * @brief Initialize the physical constants of the omni drive
 * @param R - the wheel diameter [m]
 * @param d - the radius from the center to the wheel [m]
 * @param theta - the mounting angle of the omni wheel [rad]
 * @param max_speed - the maximum speed of the robot [m/s]
 * @return omni_physical_constants_t - the physical constants of the omni drive
 */
omni_physical_constants_t init_omni(float R, float d, float theta, float max_speed)
{
    // Initialize the physical constants of the omni drive
    omni_physical_constants_t omni_physical_constants = {
        .R = R,
        .d = d,
        .max_speed = max_speed,
        .kinematics_matrix = {
            {-sin(theta) / R, cos(theta) / R, d / R},
            {-cos(theta) / R, -sin(theta) / R, d / R},
            {sin(theta) / R, -cos(theta) / R, d / R},
            {cos(theta) / R, sin(theta) / R, d / R}}};

    return omni_physical_constants;
}

/**
 * @brief Calculate the wheel velocities based on the chassis velocities
 * @param input - the input state of the omni chassis
 * @param omni_physical_constants - the physical constants of the omni drive
 */
void calculate_omni_kinematics(omni_chassis_state_t *input, omni_physical_constants_t *omni_physical_constants)
{
    float v_x = input->v_x;
    float v_y = input->v_y;
    float omega = input->omega;
    // Calculate the wheel velocities by multiplying the IK matrix by the chassis velocities
    input->phi_dot_1 = omni_physical_constants->kinematics_matrix[0][0] * v_x + omni_physical_constants->kinematics_matrix[0][1] * v_y + omni_physical_constants->kinematics_matrix[0][2] * omega;
    input->phi_dot_1 = omni_physical_constants->kinematics_matrix[1][0] * v_x + omni_physical_constants->kinematics_matrix[1][1] * v_y + omni_physical_constants->kinematics_matrix[1][2] * omega;
    input->phi_dot_1 = omni_physical_constants->kinematics_matrix[2][0] * v_x + omni_physical_constants->kinematics_matrix[2][1] * v_y + omni_physical_constants->kinematics_matrix[2][2] * omega;
    input->phi_dot_1 = omni_physical_constants->kinematics_matrix[3][0] * v_x + omni_physical_constants->kinematics_matrix[3][1] * v_y + omni_physical_constants->kinematics_matrix[3][2] * omega;
}

/**
 * @brief Scale down the wheel speeds if they exceed the maximum speed
 * @param input - the input state of the omni chassis
 * @param omni_physical_constants - the physical constants of the omni drive
 */
void desaturate_wheel_speeds(omni_chassis_state_t *input, omni_physical_constants_t *omni_physical_constants)
{
    // find the highest wheel speed
    float highest_wheel_speed = fabsf(input->phi_dot_1);
    highest_wheel_speed = fmaxf(highest_wheel_speed, fabsf(input->phi_dot_2));
    highest_wheel_speed = fmaxf(highest_wheel_speed, fabsf(input->phi_dot_3));
    highest_wheel_speed = fmaxf(highest_wheel_speed, fabsf(input->phi_dot_4));

    // scale down the wheel speeds if they exceed the maximum speed
    if (highest_wheel_speed > 0.0f)
    {
        float desaturation_coefficient = fabsf(omni_physical_constants->max_speed / highest_wheel_speed);
        input->phi_dot_1 *= desaturation_coefficient;
        input->phi_dot_2 *= desaturation_coefficient;
        input->phi_dot_3 *= desaturation_coefficient;
        input->phi_dot_4 *= desaturation_coefficient;
    }
}