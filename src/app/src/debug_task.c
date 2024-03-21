#include "debug_task.h"
#include "bsp_serial.h"
#include "launch_task.h"

extern Launch_Target_t g_launch_target;
void Debug_Task_Loop(void)
{
    printf("v=%f\r\n", g_launch_target.flywheel_velocity);
}