#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "motor_task.h"
#include "debug_task.h"
#include "bsp_serial.h"

extern void IMU_Task(void const *pvParameters);

osThreadId imu_task_handle;
osThreadId robot_control_task_handle;
osThreadId motor_task_handle;
osThreadId ui_task_handle;
osThreadId debug_task_handle;

void Robot_Tasks_Robot_Control(void const *argument);
void Robot_Tasks_Motor(void const *argument);
void Robot_Tasks_IMU(void const *argument);
void Robot_Tasks_UI(void const *argument);
void Robot_Tasks_Debug(void const *argument);

void Robot_Tasks_Start()
{
    osThreadDef(imu_task, Robot_Tasks_IMU, osPriorityAboveNormal, 0, 1024);
    imu_task_handle = osThreadCreate(osThread(imu_task), NULL);

    osThreadDef(motor_task, Robot_Tasks_Motor, osPriorityAboveNormal, 0, 256);
    motor_task_handle = osThreadCreate(osThread(motor_task), NULL);

    osThreadDef(robot_control_task, Robot_Tasks_Robot_Control, osPriorityAboveNormal, 0, 256);
    robot_control_task_handle = osThreadCreate(osThread(robot_control_task), NULL);

    osThreadDef(ui_task, Robot_Tasks_UI, osPriorityAboveNormal, 0, 256);
    ui_task_handle = osThreadCreate(osThread(ui_task), NULL);

    osThreadDef(debug_task, Robot_Tasks_Debug, osPriorityAboveNormal, 0, 256);
    debug_task_handle = osThreadCreate(osThread(debug_task), NULL);

}

void Robot_Tasks_Robot_Control(void const *argument)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(2);
    while (1)
    {
        Robot_Ctrl_Loop();
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}

__weak void Robot_Tasks_IMU(void const *argument)
{
    IMU_Task(argument);
}

void Robot_Tasks_Motor(void const *argument)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(1);
    while (1)
    {
        Motor_Task_Loop();
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}

void Robot_Tasks_UI(void const *argument)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(1);
    while (1)
    {

        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}

void Robot_Tasks_Debug(void const *argument)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(10);
    while (1)
    {
        Debug_Task_Loop();
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
