#include "launch_task.h"
#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "user_math.h"

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;
extern IMU_t g_imu;
DJI_Motor_Handle_t *g_flywheel_left, *g_flywheel_right, *g_motor_feed, *g_motor_feed;
Launch_Target_t g_launch_target;

void Feed_Angle_Calc(void);
void _launch_set_flywheel_vel_based_on_level();

void Launch_Task_Init() {
    Motor_Config_t flywheel_left_config = {
        .can_bus = 2,
        .speed_controller_id = 1,
        .offset = 0,
        .control_mode = VELOCITY_CONTROL,
        .motor_reversal = MOTOR_REVERSAL_REVERSED,
        .velocity_pid =
            {
                .kp = 500.0f,
                .output_limit = M3508_MAX_CURRENT,
            },
    };

    Motor_Config_t flywheel_right_config = {
        .can_bus = 2,
        .speed_controller_id = 2,
        .offset = 0,
        .control_mode = VELOCITY_CONTROL,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .velocity_pid =
            {
                .kp = 500.0f,
                .output_limit = M3508_MAX_CURRENT,
            },
    };

    Motor_Config_t feed_speed_config = {
        .can_bus = 1,
        .speed_controller_id = 3,
        .offset = 0,
        .control_mode = VELOCITY_CONTROL | POSITION_CONTROL,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .velocity_pid =
            {
                .kp = 500.0f,
                .output_limit = M2006_MAX_CURRENT,
            },
        .angle_pid =
            {
                .kp = 400000.0f,
                .kd = 15000000.0f,
                .ki = 0.1f,
                .output_limit = M2006_MAX_CURRENT,
                .integral_limit = 1000.0f,
            }
    };

    g_flywheel_left = DJI_Motor_Init(&flywheel_left_config,M3508);
    g_flywheel_right = DJI_Motor_Init(&flywheel_right_config,M3508);
    g_motor_feed = DJI_Motor_Init(&feed_speed_config,M2006);
}

void Launch_Ctrl_Loop() {
    if (g_robot_state.enabled) {
        if (g_launch_target.flywheel_enabled) {
            _launch_set_flywheel_vel_based_on_level();
            DJI_Motor_Set_Velocity(g_flywheel_left,g_launch_target.flywheel_velocity);
            DJI_Motor_Set_Velocity(g_flywheel_right,g_launch_target.flywheel_velocity);
            Feed_Angle_Calc();
        } else {
            DJI_Motor_Disable(g_flywheel_left);
            DJI_Motor_Disable(g_flywheel_right);
            DJI_Motor_Disable(g_motor_feed);
        }    
    } else {
        DJI_Motor_Disable(g_flywheel_left);
        DJI_Motor_Disable(g_flywheel_right);
        DJI_Motor_Disable(g_motor_feed);
    }
}

void _launch_set_flywheel_vel_based_on_level() {
    g_launch_target.flywheel_velocity = FLYWHEEL_VELOCITY_30;
}

void Feed_Angle_Calc()
{
    if (g_launch_target.single_launch_flag && !g_launch_target.single_launch_finished_flag) {
        g_launch_target.feed_angle = DJI_Motor_Get_Total_Angle(g_motor_feed) + FEED_1_PROJECTILE_ANGLE;
        g_launch_target.single_launch_finished_flag = 1;
    }
    else if (g_launch_target.single_launch_flag && g_launch_target.single_launch_finished_flag) {
        DJI_Motor_Set_Control_Mode(g_motor_feed, POSITION_CONTROL_TOTAL_ANGLE);
        DJI_Motor_Set_Angle(g_motor_feed,g_launch_target.feed_angle);
    }
    else if (g_launch_target.burst_launch_flag) {
        g_launch_target.feed_velocity = FEED_FREQUENCY_12;
        DJI_Motor_Set_Control_Mode(g_motor_feed, VELOCITY_CONTROL);
        DJI_Motor_Set_Velocity(g_motor_feed,g_launch_target.feed_velocity);
    }
    else {
        DJI_Motor_Set_Control_Mode(g_motor_feed, VELOCITY_CONTROL);
        DJI_Motor_Set_Velocity(g_motor_feed,0);
    }
}
