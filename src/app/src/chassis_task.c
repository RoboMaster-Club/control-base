
#include "chassis_task.h"

#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "Swerve_Locomotion.h"

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;
extern IMU_t g_imu;

void Chassis_Task_Init() {
    Swerve_Init();
}

void Chassis_Ctrl_Loop() {
    if (g_robot_state.enabled) {
        Swerve_Drive(g_remote.controller.left_stick.x / REMOTE_STICK_MAX,
                    g_remote.controller.left_stick.y / REMOTE_STICK_MAX, 
                    -g_remote.controller.right_stick.x / REMOTE_STICK_MAX);
    } else {
        Swerve_Disable();
    }
}
