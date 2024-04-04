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
#include "bsp_daemon.h"

extern Robot_State_t g_robot_state;
extern DJI_Motor_Handle_t *g_yaw;
extern IMU_t g_imu;
extern Swerve_Module_t g_swerve_fl;
extern Remote_t g_remote; 
extern Launch_Target_t g_launch_target;
extern uint64_t t;
extern Daemon_Instance_t *g_daemon_instances[3];

void Debug_Task_Loop(void)
{
    static uint32_t counter = 0;
    // DEBUG_PRINTF(&huart6, "time=%.1f,a=%d,b=%d,c=%f,d=%d\r\n", (float) counter / 1000.0f * DEBUG_PERIOD, 
    //             g_daemon_instances[1]->counter, g_daemon_instances[0]->counter, g_imu.deg.roll, counter);
    counter++;
    if (counter > 0xFFFFFFFF) {
        counter = 0;
    }
}