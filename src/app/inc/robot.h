#ifndef ROBOT_H
#define ROBOT_H

#include <stdint.h>

typedef struct
{
  uint8_t enabled;
  uint8_t safely_started;
  uint8_t spintop_mode;
  uint8_t autoaiming_enabled;
  float gimbal_pitch_angle;
  float gimbal_yaw_angle;
  float vx;
  float vy;
  float vx_keyboard;
  float vy_keyboard;
} Robot_State_t;

typedef struct
{
  float x_speed;
  float y_speed;
  float omega;
  float max_speed;
  float power_buffer_frac;
  float current_draw_frac;
  float speed_limiter;
} Chassis_State_t;

typedef struct
{
  uint8_t prev_B;
  uint8_t prev_G;
  uint8_t prev_left_switch;
} Key_Prev_t;

void Robot_Init(void);
void Robot_Ctrl_Loop(void);

#endif // ROBOT_H
