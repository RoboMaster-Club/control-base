#include "motor_task.h"
#include "dji_motor.h"
#include "dm_motor.h"
#include "mf_motor.h"

void Motor_Task_Loop() {
    DJI_Motor_Send();
    MF_Motor_Send();
    DM_Motor_Send();
}

