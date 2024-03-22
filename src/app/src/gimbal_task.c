#include "gimbal_task.h"
#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "user_math.h"
#include "jetson_orin.h"
#include <math.h>
extern Robot_State_t g_robot_state;
extern Remote_t g_remote;
extern IMU_t g_imu;
float gimbal_test = 0;
uint64_t t = 0;
DJI_Motor_Handle_t *g_yaw, *g_pitch;
Gimbal_Target_t g_gimbal_target;

void Gimbal_Task_Init()
{
    Motor_Config_t yaw_motor_config = {
        .can_bus = 1,
        .speed_controller_id = 3,
        .offset = 3690,
        .control_mode = POSITION_VELOCITY_SERIES,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .use_external_feedback = 1,
        .external_feedback_dir = 1,
        .external_angle_feedback_ptr = &g_imu.rad.yaw,
        .external_velocity_feedback_ptr = &(g_imu.bmi088_raw.gyro[2]),
        .angle_pid =
            {
                .kp = 25.0f,
                .kd = 100.0f,
                .output_limit = 25,
            },
        .velocity_pid =
            {
                .kp = 5000.0f,
                .ki = 0.0f,
                .kf = 1000.0f,
                .feedforward_limit = 5000.0f,
                .integral_limit = 5000.0f,
                .output_limit = GM6020_MAX_CURRENT,
            },
    };

    #pragma message "CHANGE PITCH MOTOR OFFSET DO NO TRUN BEFORE DOING THIS"
    int a = 0;
    Motor_Config_t pitch_motor_config = {
        .can_bus = 2,
        .speed_controller_id = 2,
        .offset = 1500,
        .use_external_feedback = 1,
        .external_feedback_dir = -1,
        .external_angle_feedback_ptr = &g_imu.rad.roll, // pitch
        .external_velocity_feedback_ptr = &(g_imu.bmi088_raw.gyro[0]),
        .control_mode = POSITION_VELOCITY_SERIES,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .angle_pid =
            {
                .kp = 25.0f,
                .kd = 100.0f,
                .output_limit = 50.0f,
            },
        .velocity_pid =
            {
                .kp = 4500.0f,
                .ki = 0.8f,
                .integral_limit = 4000.0f,
                .output_limit = GM6020_MAX_CURRENT,
            },
    };

    g_yaw = DJI_Motor_Init(&yaw_motor_config, GM6020);
    g_pitch = DJI_Motor_Init(&pitch_motor_config, GM6020);
}

void Gimbal_Ctrl_Loop()
{
    if (g_robot_state.enabled)
    {
        __MAX_LIMIT(g_gimbal_target.pitch, -0.45f, 0.5f);
        DJI_Motor_Set_Angle(g_pitch, g_robot_state.gimbal_pitch_angle);
        DJI_Motor_Set_Angle(g_yaw, g_robot_state.gimbal_yaw_angle);
    }
    else
    {
        DJI_Motor_Disable(g_yaw);
        DJI_Motor_Disable(g_pitch);
    }
}
