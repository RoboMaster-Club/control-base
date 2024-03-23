#ifndef ROBOT_H
#define ROBOT_H

#include <stdint.h>

typedef struct {
  uint8_t enabled;
  uint8_t spintop_mode;
  float chassis_x_speed;
  float chassis_y_speed;
  float chassis_omega;
  float gimbal_pitch_angle;
  float gimbal_yaw_angle;
  float vx;
  float vy;
} Robot_State_t;

void Robot_Init(void);
void Robot_Ctrl_Loop(void);

#endif // ROBOT_H
