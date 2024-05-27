#include "Swerve_Locomotion.h"
#include <stdbool.h>
#include <stdlib.h>
#include "user_math.h"
#include "dji_motor.h"

Swerve_Module_t g_swerve_fl, g_swerve_rl, g_swerve_rr, g_swerve_fr;
Swerve_Module_t *swerve_modules[NUMBER_OF_MODULES] = {&g_swerve_fl, &g_swerve_rl, &g_swerve_rr, &g_swerve_fr};
float last_swerve_angle[NUMBER_OF_MODULES] = {0.0f, 0.0f, 0.0f, 0.0f};
Kalman_Filter_t power_KF = {.Prev_P = 1.0f, .Q = 0.0001, .R = 5.0f};

// #define SWERVE_OPTIMIZE

/**
 * @brief Inverse kinematics matrix for a 4 module swerve, defined counterclockwise from the front left
 *
 * This matrix represents the inverse kinematics for a 4 module swerve drive system. It is used to calculate the desired
 * wheel speeds and angles based on the desired robot velocity and rotation. The matrix is defined in a counterclockwise
 * order starting from the front left module.
 *
 * The values in the matrix are as follows:
 * - Row 0-1: Front left module (ID 1)
 * - Row 2-3: Rear left module (ID 2)
 * - Row 4-5: Rear right module (ID 3)
 * - Row 6-7: Front right module (ID 4)
 *
 * @note The values WHEEL_BASE and TRACK_WIDTH are used to calculate the offsets in the matrix.
 */
float swerve_state_matrix[8][3] = {
    {0, 1, -(WHEEL_BASE / 2)}, // front left id 1
    {1, 0, -(TRACK_WIDTH / 2)},
    {0, 1, -(WHEEL_BASE / 2)}, // rear left id 2
    {1, 0, +(TRACK_WIDTH / 2)},
    {0, 1, +(WHEEL_BASE / 2)}, // rear right id 3
    {1, 0, +(TRACK_WIDTH / 2)},
    {0, 1, +(WHEEL_BASE / 2)}, // front right id 4
    {1, 0, -(TRACK_WIDTH / 2)}};

void Swerve_Init(void);
void Swerve_Processing(void);
void Set_Module_Output(Swerve_Module_t *swerve_module, Module_State_t desired_state);

/* Initialize physical constants of each module */
void Swerve_Init()
{
    // define constants for each module in an array [0] == fl, [1] == rl, [2] == rr, [3] == fr
    int azimuth_can_bus_array[NUMBER_OF_MODULES] = {1, 1, 2, 2};
    int azimuth_speed_controller_id_array[NUMBER_OF_MODULES] = {1, 2, 3, 4};
    int azimuth_offset_array[NUMBER_OF_MODULES] = {1990, 730, 6060, 1362};
    Motor_Reversal_t azimuth_motor_reversal_array[NUMBER_OF_MODULES] = {MOTOR_REVERSAL_REVERSED, MOTOR_REVERSAL_REVERSED, MOTOR_REVERSAL_REVERSED, MOTOR_REVERSAL_REVERSED};

    int drive_can_bus_array[NUMBER_OF_MODULES] = {1, 1, 2, 2};
    int drive_speed_controller_id_array[NUMBER_OF_MODULES] = {1, 2, 3, 4};
    Motor_Reversal_t drive_motor_reversal_array[NUMBER_OF_MODULES] = {MOTOR_REVERSAL_REVERSED, MOTOR_REVERSAL_REVERSED, MOTOR_REVERSAL_NORMAL, MOTOR_REVERSAL_NORMAL};

    // init common PID configuration for azimuth motors
    Motor_Config_t azimuth_motor_config = {
        .control_mode = POSITION_CONTROL,
        .angle_pid = {
            .kp = 15000.0f,
            .kd = 8000.0f,
            .kf = 5000.0f,
            .output_limit = GM6020_MAX_CURRENT,
        }};

    // init common PID configuration for drive motors
    Motor_Config_t drive_motor_config = {
        .control_mode = VELOCITY_CONTROL,
        .velocity_pid = {
            .kp = 500.0f,
            .ki = 5.0f,
            .output_limit = M3508_MAX_CURRENT,
            .integral_limit = 3000.0f,
        }};

    for (int i = 0; i < NUMBER_OF_MODULES; i++)
    {
        // configure azimuth motor
        azimuth_motor_config.can_bus = azimuth_can_bus_array[i];
        azimuth_motor_config.offset = azimuth_offset_array[i];
        azimuth_motor_config.speed_controller_id = azimuth_speed_controller_id_array[i];
        azimuth_motor_config.motor_reversal = azimuth_motor_reversal_array[i];
        swerve_modules[i]->azimuth_motor = DJI_Motor_Init(&azimuth_motor_config, GM6020);

        // configure drive motor
        drive_motor_config.can_bus = drive_can_bus_array[i];
        drive_motor_config.speed_controller_id = drive_speed_controller_id_array[i];
        drive_motor_config.motor_reversal = drive_motor_reversal_array[i];
        swerve_modules[i]->drive_motor = DJI_Motor_Init(&drive_motor_config, M3508);
    }
}

/* Scale wheel speeds to max possible speed while preserving ratio between modules.*/
Module_State_Array_t Desaturate_Wheel_Speeds(Module_State_Array_t module_state_array)
{
    float highest_speed = fabsf(module_state_array.states[0].speed);
    for (int i = 1; i < NUMBER_OF_MODULES; i++) // start from 1 to find the highest speed
    {
        highest_speed = fmaxf(highest_speed, fabsf(module_state_array.states[i].speed));
    }
    if (highest_speed > 0.01f) // avoid division by zero
    {
        float desaturation_coefficient = fabsf(SWERVE_MAX_SPEED / highest_speed);
        Module_State_Array_t desaturated_module_states = {0}; // initialize the struct to zero

        for (int i = 0; i < NUMBER_OF_MODULES; i++)
        {
            desaturated_module_states.states[i].speed = module_state_array.states[i].speed * desaturation_coefficient;
            desaturated_module_states.states[i].angle = module_state_array.states[i].angle;
        }

        return desaturated_module_states;
    }
    return module_state_array;
}

/**
 * Convert chassis speeds to module states using inverse kinematics.
 *
 * @param chassis_speeds The chassis speeds in meters per second in the x, y, and omega directions.
 * @return The calculated module states.
 */
Module_State_Array_t Chassis_Speeds_To_Module_States(Chassis_Speeds_t chassis_speeds)
{
    Module_State_Array_t calculated_module_states = {0};
    if (chassis_speeds.x == 0 && chassis_speeds.y == 0 && chassis_speeds.omega == 0)
    {
        for (int i = 0; i < NUMBER_OF_MODULES; i++)
        {
            calculated_module_states.states[i].speed = 0;
            calculated_module_states.states[i].angle = last_swerve_angle[i];
        }
    }
    else
    {
        // Multiply the inverse kinematics matrix by the chassis speeds vector
        float module_states_matrix[8] = {
            swerve_state_matrix[0][0] * chassis_speeds.x + swerve_state_matrix[0][1] * chassis_speeds.y + swerve_state_matrix[0][2] * chassis_speeds.omega,
            swerve_state_matrix[1][0] * chassis_speeds.x + swerve_state_matrix[1][1] * chassis_speeds.y + swerve_state_matrix[1][2] * chassis_speeds.omega,
            swerve_state_matrix[2][0] * chassis_speeds.x + swerve_state_matrix[2][1] * chassis_speeds.y + swerve_state_matrix[2][2] * chassis_speeds.omega,
            swerve_state_matrix[3][0] * chassis_speeds.x + swerve_state_matrix[3][1] * chassis_speeds.y + swerve_state_matrix[3][2] * chassis_speeds.omega,
            swerve_state_matrix[4][0] * chassis_speeds.x + swerve_state_matrix[4][1] * chassis_speeds.y + swerve_state_matrix[4][2] * chassis_speeds.omega,
            swerve_state_matrix[5][0] * chassis_speeds.x + swerve_state_matrix[5][1] * chassis_speeds.y + swerve_state_matrix[5][2] * chassis_speeds.omega,
            swerve_state_matrix[6][0] * chassis_speeds.x + swerve_state_matrix[6][1] * chassis_speeds.y + swerve_state_matrix[6][2] * chassis_speeds.omega,
            swerve_state_matrix[7][0] * chassis_speeds.x + swerve_state_matrix[7][1] * chassis_speeds.y + swerve_state_matrix[7][2] * chassis_speeds.omega,
        };

        // Convert module x,y matrix to wheel speed and angle
        for (int i = 0; i < NUMBER_OF_MODULES; i++)
        {
            float x = module_states_matrix[i * 2 + 1];
            float y = module_states_matrix[i * 2];
            float speed = hypotf(x, y);

            calculated_module_states.states[i].speed = speed;

            if (speed > 1e-6f)
            {
                float angle = atan2f(y, x);
                calculated_module_states.states[i].angle = angle;
                last_swerve_angle[i] = angle;
            }
            else
            {
                calculated_module_states.states[i].angle = last_swerve_angle[i];
            }
        }
    }
    return calculated_module_states;
}

/**
 * @brief Optimize Module Angle
 * Optimize the module angle and velocity to minimize the movement of the module
 * @param input_state - the target state
 * @param measured_angle - the current angle
 * @return Module_State_t - the optimized state
 */
Module_State_t Optimize_Module_Angle(Module_State_t input_state, float measured_angle)
{
    Module_State_t optimized_module_state = {0};
    optimized_module_state.speed = input_state.speed;
    float target_angle = fmodf(input_state.angle, 2.0f * PI); // get positive normalized target angle
    if (target_angle < 0)
    {
        target_angle += 2.0f * PI;
    }

    float curr_angle_normalized = fmodf(measured_angle, 2 * PI); // get normalized current angle
    if (curr_angle_normalized < 0)
    {
        curr_angle_normalized += 2.0f * PI;
    }

    float delta = target_angle - curr_angle_normalized;          // calculate the angle delta

    // these conditions essentially create the step function for angles to "snap" too offset from the measured angle to avoid flipping
    if (PI / 2.0f < fabsf(delta) && fabsf(delta) < 3 * PI / 2.0f)
    {
        float beta = PI - fabsf(delta);
        beta *= (delta > 0 ? 1.0f : -1.0f);
        target_angle = measured_angle - beta;
        optimized_module_state.speed = -1.0f * input_state.speed; // invert velocity if optimal angle 180 deg from target
    }
    else if (fabsf(delta) >= 3.0f * PI / 2.0f)
    {
        if (delta < 0)
        {
            target_angle = measured_angle + (2.0f * PI + delta);
        }
        else
        {
            target_angle = measured_angle - (2.0f * PI - delta);
        }
    }
    else
    {
        target_angle = measured_angle + delta;
    }

    optimized_module_state.angle = target_angle;
    return optimized_module_state;
}

/**
 * Sets the output of a swerve module to the desired state.
 *
 * @param swerve_module The pointer to the swerve module to set the output for.
 * @param desired_state The desired state of the swerve module.
 */
void Set_Module_Output(Swerve_Module_t *swerve_module, Module_State_t desired_state)
{
#ifdef SWERVE_OPTIMIZE    
    Module_State_t optimized_module_state = Optimize_Module_Angle(desired_state, DJI_Motor_Get_Absolute_Angle(swerve_module->azimuth_motor));
    DJI_Motor_Set_Angle(swerve_module->azimuth_motor, optimized_module_state.angle);
    DJI_Motor_Set_Velocity(swerve_module->drive_motor, optimized_module_state.speed * 60 / (PI * Wheel_Diameter));
#else
    DJI_Motor_Set_Angle(swerve_module->azimuth_motor, desired_state.angle);
    DJI_Motor_Set_Velocity(swerve_module->drive_motor, desired_state.speed * 60 / (PI * Wheel_Diameter));
#endif
}

/* Set the desired modules state of each module */
void Set_Desired_States(Module_State_Array_t desired_states)
{
    g_swerve_fl.module_state = desired_states.states[0];
    g_swerve_rl.module_state = desired_states.states[1];
    g_swerve_rr.module_state = desired_states.states[2];
    g_swerve_fr.module_state = desired_states.states[3];
}

/**
 * Takes inputs from (-1 to 1) and sets the respective module outputs.
 *
 * @param x: x speed of the robot
 * @param y: y speed of the robot
 * @param omega: angular speed of the robot
 */
void Swerve_Drive(float x, float y, float omega)
{
    x *= SWERVE_MAX_SPEED; // convert to m/s
    y *= SWERVE_MAX_SPEED;
    omega *= SWERVE_MAX_ANGLUAR_SPEED; // convert to rad/s
    #ifdef POWER_CONTROL
        if(fabs(x) > 0.1f || fabs(y) > 0.1f || fabs(omega) > 0.1f)
        {
            __MOVING_AVERAGE(g_robot_state.chassis_power_buffer,g_robot_state.chassis_power_index,
            Referee_Robot_State.Chassis_Power,g_robot_state.chassis_power_count,g_robot_state.chassis_total_power,g_robot_state.chassis_avg_power);
            if(g_robot_state.chassis_avg_power < (Referee_Robot_State.Chassis_Power_Max*0.7f))
                g_robot_state.power_increment_ratio += 0.001f;
            else
                g_robot_state.power_increment_ratio -= 0.001f;
        }
        else
        {
            memset(g_robot_state.chassis_power_buffer,0,sizeof(g_robot_state.chassis_power_buffer));
            g_robot_state.chassis_power_index = 0;
            g_robot_state.chassis_power_count = 0;
            g_robot_state.chassis_total_power = 0;
            g_robot_state.power_increment_ratio = 1.0f;
        }
        __MAX_LIMIT(g_robot_state.power_increment_ratio, 0.8f, 5.0f);
        x *= g_robot_state.power_increment_ratio; // convert to m/s
        y *= g_robot_state.power_increment_ratio;
        omega *= g_robot_state.power_increment_ratio; // convert to rad/s
    #endif
    Chassis_Speeds_t desired_chassis_speeds = {.x = x, .y = y, .omega = omega};
    Set_Desired_States(Chassis_Speeds_To_Module_States(desired_chassis_speeds));

    for (int i = 0; i < NUMBER_OF_MODULES; i++)
    {
        Set_Module_Output(swerve_modules[i], swerve_modules[i]->module_state);
    }
}

/* Commands modules to stop moving and reset angle to 0. Should be called on robot enable */
void Reset_Modules()
{
    Module_State_Array_t desired_states = {
        .states = {
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.0f}}};
    Set_Desired_States(desired_states);
}

/**
 * Disables all the motors used in the swerve.
 */
void Swerve_Disable()
{
    Chassis_Speeds_t desired_chassis_speeds = {.x = 0.0f, .y = 0.0f, .omega = 0.0f};
    Set_Desired_States(Chassis_Speeds_To_Module_States(desired_chassis_speeds));
    DJI_Motor_Disable(g_swerve_fr.azimuth_motor);
    DJI_Motor_Disable(g_swerve_fr.drive_motor);
    DJI_Motor_Disable(g_swerve_fl.azimuth_motor);
    DJI_Motor_Disable(g_swerve_fl.drive_motor);
    DJI_Motor_Disable(g_swerve_rr.azimuth_motor);
    DJI_Motor_Disable(g_swerve_rr.drive_motor);
    DJI_Motor_Disable(g_swerve_rl.azimuth_motor);
    DJI_Motor_Disable(g_swerve_rl.drive_motor);
}
