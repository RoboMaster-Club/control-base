#ifndef WHEEL_LEGGED_LOCOMOTION_H
#define WHEEL_LEGGED_LOCOMOTION_H

#include <stdint.h>
#include "arm_math.h"

#ifdef __cplusplus
extern "C" {
#endif

// #define THREE_D_MODEL
#define TWO_D_MODEL

#ifdef TWO_D_MODEL

typedef struct _Leg {
    float state_vector[6];
} Wheel_Leg_t;

#endif

#ifdef __cplusplus
}
#endif

#endif // WHEEL_LEGGED_LOCOMOTION_H
