#include "robot.h"

#include "robot_tasks.h"
#include "chassis_task.h"
#include "gimbal_task.h"
#include "launch_task.h"
#include "remote.h"
#include "bsp_can.h"

Robot_State_t g_robot_state = {0, 0};
extern Remote_t g_remote;

void Robot_Cmd_Loop(void);

void Robot_Init() {
    // Initialize all hardware
    //Chassis_Task_Init();
    Gimbal_Task_Init();
    Launch_Task_Init();
    Remote_Init();
    CAN_Service_Init();
    // Initialize all tasks
    Robot_Tasks_Start();
}

void Robot_Ctrl_Loop() {
    // Control loop for the robot
    Robot_Cmd_Loop();
    //Chassis_Ctrl_Loop();
    Gimbal_Ctrl_Loop();
    Launch_Ctrl_Loop();
}

void Robot_Cmd_Loop() {
    if (g_remote.controller.right_switch == DOWN) {
        g_robot_state.enabled = 0;
    } else if (g_remote.controller.right_switch == MID){
        g_robot_state.enabled = 1;
    }
}
