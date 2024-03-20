#ifndef ROBOT_H
#define ROBOT_H

#include <stdint.h>

typedef struct {
  uint8_t enabled;
  uint8_t mode;
  uint8_t chassis_enabled;
  float chassis_x_speed;
  float chassis_y_speed;
  float chassis_omega;
  uint8_t gimbal_enabled;
  uint8_t launch_enabled;
} Robot_State_t;

void Robot_Init(void);
void Robot_Ctrl_Loop(void);
void Controller_Update();
void Mouse_Update();
void Keyboard_Update();

#endif // ROBOT_H
