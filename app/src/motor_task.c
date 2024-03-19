#include "motor_task.h"
#include "dji_motor.h"

void Motor_Task_Loop() {
    DJI_Motor_Send();
}

