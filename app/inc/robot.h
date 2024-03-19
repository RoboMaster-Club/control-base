#ifndef ROBOT_H
#define ROBOT_H

#include <stdint.h>

typedef struct {
  uint8_t enabled;
  uint8_t mode;
} Robot_State_t;

void Robot_Init(void);
void Robot_Ctrl_Loop(void);

#endif // ROBOT_H
