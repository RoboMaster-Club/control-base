#include "chassis_task.h"

#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "Swerve_Locomotion.h"
#include "unitree_motor.h"

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;
extern IMU_t g_imu;

void Chassis_Task_Init()
{
    // Swerve_Init();
    Unitree_Motor_Config_t unitree_motor_config = {
        .motor_id = 0,
        .k_pos = 0.0f,
        .k_vel = 0.0f,
        .huartx = &huart1,
    };
}

void Chassis_Ctrl_Loop()
{

    if (g_robot_state.enabled)
    {
        
    }
    else
    {
        //Unitree_Motor_Disable();
    }
}