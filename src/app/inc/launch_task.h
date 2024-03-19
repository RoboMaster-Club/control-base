#ifndef LAUNCH_TASK_H
#define LAUNCH_TASK_H

#include <stdint.h>

#define FLYWHEEL_VELOCITY_30 (2.0f )
#define FEED_HOLE_NUM (6.0f)
#define FEED_1_PROJECTILE_ANGLE (2.0f*PI/M2006_REDUCTION_RATIO/FEED_HOLE_NUM)
#define FEED_FREQUENCY_6 (6.0f / FEED_HOLE_NUM * 60.0f)
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
} Launch_Target_t;

void Launch_Task_Init(void);
void Launch_Ctrl_Loop(void);

#endif // LAUNCH_TASK_H
