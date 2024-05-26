#ifndef LAUNCH_TASK_H
#define LAUNCH_TASK_H

#include <stdint.h>
#include "dji_motor.h"

#define FLYWHEEL_VELOCITY_23 (6000.0f * M3508_REDUCTION_RATIO)
#define FLYWHEEL_VELOCITY_30 (7000.0f * M3508_REDUCTION_RATIO)
#define FEED_HOLE_NUM (6.0f)
#define FEED_1_PROJECTILE_ANGLE (2.0f*PI/FEED_HOLE_NUM)
#define FEED_FREQUENCY_6 (6.0f / FEED_HOLE_NUM * 60.0f)
#define FEED_FREQUENCY_12 (12.0f / FEED_HOLE_NUM * 60.0f)
#define FEED_FREQUENCY_18 (18.0f / FEED_HOLE_NUM * 60.0f)
#define FEED_FREQUENCY_30 (30.0f / FEED_HOLE_NUM * 60.0f)

typedef struct
{
    float flywheel_velocity;
    float feed_angle;
    float feed_velocity;
    uint8_t single_launch_flag;
    uint8_t single_launch_finished_flag;
    uint8_t burst_launch_flag;
    uint8_t flywheel_enabled;
} Launch_Target_t;

void Launch_Task_Init(void);
void Launch_Ctrl_Loop(void);

extern Launch_Target_t g_launch_target;

#endif // LAUNCH_TASK_H
