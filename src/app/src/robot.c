#include "robot.h"

#include "robot_tasks.h"
#include "chassis_task.h"
#include "gimbal_task.h"
#include "launch_task.h"
#include "remote.h"
#include "bsp_can.h"

Robot_State_t g_robot_state = {0, 0, 0, 0, 0};
extern Remote_t g_remote;

void Robot_Cmd_Loop(void);

void Robot_Init()
{
    // Initialize all hardware
    // Chassis_Task_Init();
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
    // Gimbal_Ctrl_Loop();
    Launch_Ctrl_Loop();
}

void Robot_Cmd_Loop()
{
    Controller_Update();
    Mouse_Update();
    Keyboard_Update();
}

void Controller_Update()
{
    /* Disable the robot if both switches are down */
    if (g_remote.controller.right_switch == DOWN && g_remote.controller.left_switch == DOWN) // TODO disable robot if no data from remote
    {
        g_robot_state.enabled = 0;
    }
    else
    {
        /* When the right switch is mid */
        if (g_remote.controller.right_switch == MID)
        {
            /* When the left switch is up enable launch, and enter spin top */
            if (g_remote.controller.left_switch == UP)
            {
                g_robot_state.chassis_enabled = 0; // TODO enter spin top mode
                g_robot_state.gimbal_enabled = 1;
                g_robot_state.launch_enabled = 1;
            }
            else
            {
                /* The chassis is controlled by the sticks */
                g_robot_state.chassis_enabled = 1;
                g_robot_state.chassis_x_speed = g_remote.controller.left_stick.x / REMOTE_STICK_MAX;
                g_robot_state.chassis_y_speed = g_remote.controller.left_stick.y / REMOTE_STICK_MAX;
                g_robot_state.chassis_x_speed = -g_remote.controller.right_stick.x / REMOTE_STICK_MAX;

                g_robot_state.gimbal_enabled = 0;
                g_robot_state.launch_enabled = 0;
            }
        }

        /* Enable the robot */
        g_robot_state.enabled = 1;
    }
}

void Mouse_Update()
{
}

void Keyboard_Update()
{

    float x_speed = 0, y_speed = 0, omega = 0;
    if (g_remote.keyboard.W == 1)
    {
        x_speed += 1;
    }
    if (g_remote.keyboard.S == 1)
    {
        y_speed += -1;
    }
    if (g_remote.keyboard.A == 1)
    {
        y_speed += -1;
    }
    if (g_remote.keyboard.D == 1)
    {
        y_speed += 1;
    }
    if (g_remote.keyboard.Q == 1)
    {
        omega += -1;
    }
    if (g_remote.keyboard.E == 1)
    {
        omega += 1;
    }
    g_robot_state.chassis_x_speed = x_speed;
    g_robot_state.chassis_y_speed = y_speed;
    g_robot_state.chassis_omega = omega;

    /*This approach could cause physical damage to the motors due to instantatneous polarity changes*/
}
