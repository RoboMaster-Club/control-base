#include "robot.h"

#include "robot_tasks.h"
#include "chassis_task.h"
#include "gimbal_task.h"
#include "launch_task.h"
#include "remote.h"
#include "bsp_can.h"
#include "gimbal_task.h"
#include <math.h>

extern DJI_Motor_Handle_t *g_yaw;
#define SPIN_TOP_OMEGA (1.0f)

#define KEYBOARD_RAMP_COEF (0.2f)
#define CONTROLLER_RAMP_COEF (0.8f)

Robot_State_t g_robot_state = {0, 0};
extern Remote_t g_remote;

void Robot_Cmd_Loop(void);

void Robot_Init()
{
    // Initialize all hardware
    Chassis_Task_Init();
    Gimbal_Task_Init();
    Launch_Task_Init();
    Remote_Init();
    CAN_Service_Init();
    // Initialize all tasks
    Robot_Tasks_Start();
}

void Robot_Ctrl_Loop()
{
    // Control loop for the robot
    Robot_Cmd_Loop();
    Chassis_Ctrl_Loop();
    Gimbal_Ctrl_Loop();
    Launch_Ctrl_Loop();
}

void Robot_Cmd_Loop()
{
    if (g_remote.controller.right_switch == DOWN)
    {
        g_robot_state.enabled = 0;
    }
    else
    {
        g_robot_state.enabled = 1;

        g_robot_state.vx += ((1.0f - KEYBOARD_RAMP_COEF) * g_robot_state.vx + g_remote.keyboard.W * KEYBOARD_RAMP_COEF - g_remote.keyboard.S * KEYBOARD_RAMP_COEF) +
                            ((1.0f - CONTROLLER_RAMP_COEF) * g_robot_state.vx + (g_remote.controller.left_stick.x / REMOTE_STICK_MAX) * CONTROLLER_RAMP_COEF);
        g_robot_state.vy += ((1.0f - KEYBOARD_RAMP_COEF) * g_robot_state.vy + g_remote.keyboard.A * KEYBOARD_RAMP_COEF - g_remote.keyboard.D * KEYBOARD_RAMP_COEF) +
                            ((1.0f - CONTROLLER_RAMP_COEF) * g_robot_state.vy + (g_remote.controller.left_stick.y / REMOTE_STICK_MAX) * CONTROLLER_RAMP_COEF);

        float theta = DJI_Motor_Get_Absolute_Angle(g_yaw);
        g_robot_state.chassis_x_speed = -g_robot_state.vy * sin(theta) + g_robot_state.vx * cos(theta);
        g_robot_state.chassis_y_speed = g_robot_state.vy * cos(theta) + g_robot_state.vx * sin(theta);
        
        if (g_remote.controller.left_switch == DOWN)
        {
            g_robot_state.chassis_omega = 0;
        }
        else if (g_remote.controller.left_switch == MID)
        {
            g_robot_state.chassis_omega = SPIN_TOP_OMEGA;
        }
        else if (g_remote.controller.left_switch == UP)
        {
            // RESERVED        }
        }

        if (g_remote.controller.right_switch == MID)
        {
            g_robot_state.gimbal_yaw_angle -= (g_remote.controller.right_stick.x / 50000.0f + g_remote.mouse.x);    // controller and mouse
            g_robot_state.gimbal_pitch_angle -= (g_remote.controller.right_stick.y / 100000.0f + g_remote.mouse.y); // controller and mouse
        }
        else if (g_remote.controller.right_switch == UP)
        {
            // TODO: Algorithm Auto Aiming
        }
    }
}
