#include "debug_task.h"
#include "debugger.h"
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
extern Daemon_Instance_t *g_remote_daemon;
#define PRINT_RUNTIME_STATS
#ifdef PRINT_RUNTIME_STATS
char g_debug_buffer[1024 * 2] = {0};
#endif

const char *top_border = "\r\n\r\n\r\n/***** System Info *****/\r\n";
const char *bottom_border = "/***** End of Info *****/\r\n";

#define DEBUG_ENABLED 1

void Debug_Task_Init(void)
{
}

void Debug_Task_Loop(void)
{
    #ifdef DEBUG_ENABLED
        static uint32_t counter = 0;
        #ifdef PRINT_RUNTIME_STATS
            if (counter % 100 == 0) // Print every 100 cycles
            {
                vTaskGetRunTimeStats(g_debug_buffer);
                Debugger_Log_Data("%s", top_border);
                Debugger_Log_Data("%s", g_debug_buffer);
                Debugger_Log_Data("%s", bottom_border);
            }
        #endif
        Debugger_Log_Data(">time:%.1f\n>yaw:%f\n>pitch:%f\n>roll:%f\n", (float) counter / 1000.0f * DEBUG_PERIOD,
        g_imu.deg.yaw, g_imu.deg.pitch, g_imu.deg.roll);

        //Debugger_Log_Data(">yaw:%f\n", g_imu.deg.yaw);
        Debugger_Log_Data(">remote_daemon:%d\n", g_remote_daemon->counter);
        counter++;
        if (counter > 0xFFFFFFFF)
        {
            counter = 0;
        }
    #endif
}

