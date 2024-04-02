
#include "chassis_task.h"

#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "Swerve_Locomotion.h"
#include "dm4310.h"

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;
extern IMU_t g_imu;

DM_Motor_t *g_hip_left_front;

void Chassis_Task_Init() {
    DM_Motor_Config_t motor_config = {
        .can_bus = 1,
        .rx_id = 0x301,
        .tx_id = 0x201,
        .control_mode = DM_MOTOR_MIT,
        .kp = 8.0f,
        .kd = 1.0f
    };
    g_hip_left_front = DM_Motor_Init(&motor_config);
}

void Chassis_Ctrl_Loop() {
    
    static float target_pos = 0.0f;
    target_pos += 0.001f;
    DM_Motor_Ctrl_MIT(g_hip_left_front, target_pos, 0, 0);
    if (g_robot_state.enabled) {
        
    } else {
        
    }
}
