#ifndef CHASSIS_TASK_H
#define CHASSIS_TASK_H

#include "motion_profile.h"

extern Trapezoid_Profile_t x_speed_profile;
extern Trapezoid_Profile_t y_speed_profile;
extern Trapezoid_Profile_t omega_profile;

// Function prototypes
void Chassis_Task_Init(void);
void Chassis_Ctrl_Loop(void);

#endif // CHASSIS_TASK_H
