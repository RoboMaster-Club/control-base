#include "debug_task.h"
#include "bsp_serial.h"
#include "launch_task.h"
#include "remote.h"
#include "gimbal_task.h"
#include "Swerve_Locomotion.h"
#include "user_math.h"
#include "imu_task.h"
#include "robot.h"
#include "referee_system.h"
#include "jetson_orin.h"

extern Robot_State_t g_robot_state;
extern DJI_Motor_Handle_t *g_yaw;
extern IMU_t g_imu;
extern Swerve_Module_t g_swerve_fl;
extern Remote_t g_remote; 
extern Launch_Target_t g_launch_target;
extern uint64_t t;
void Debug_Task_Loop(void)
{
    printf("a=%f,b=%f\r\n", g_orin_data.receiving.auto_aiming.yaw, g_orin_data.receiving.auto_aiming.pitch);
}