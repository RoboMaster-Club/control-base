#include "dm4310.h"
#include <stdlib.h>

#define DM_MAX_DEVICE (10)
DM_Motor_t *g_dm_motors[DM_MAX_DEVICE] = {NULL};

/**
 * int float_to_uint(float x, float x_min, float x_max, int bits)
 *
 * convert a float into int by spanning the range of the variable and
 * linearly assign the value
 *
 * Input:
 * float x: the float variable to be converted into int
 * float x_min: the min value of x
 * float x_max: the max value of x
 * int bits: bit number of converted int
 *
 * Output:
 * int: the converted int
 */
int float_to_uint(float x, float x_min, float x_max, int bits)
{
    float span = x_max - x_min;
    float offset = x_min;
    return (int)((x - offset) * ((float)((1 << bits) - 1)) / span);
}

float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
    /// converts unsigned int to float, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}

void DM_Motor_Decode(CAN_Instance_t *motor_can_instance)
{
    uint8_t *data = motor_can_instance->rx_buffer;
    DM_Motor_Stats_t *data_frame = (DM_Motor_Stats_t *)motor_can_instance->binding_motor_stats;

    data_frame->id = (data[0]) & 0x0F;
    data_frame->state = (data[0]) >> 4;
    data_frame->pos_int = (data[1] << 8) | data[2];
    data_frame->vel_int = (data[3] << 4) | (data[4] >> 4);
    data_frame->torq_int = ((data[4] & 0xF) << 8) | data[5];
    data_frame->pos = uint_to_float(data_frame->pos_int, P_MIN, P_MAX, 16);   // (-12.5,12.5)
    data_frame->vel = uint_to_float(data_frame->vel_int, V_MIN, V_MAX, 12);   // (-45.0,45.0)
    data_frame->torq = uint_to_float(data_frame->torq_int, T_MIN, T_MAX, 12); // (-18.0,18.0)
    data_frame->t_mos = (float)(data[6]);
    data_frame->t_rotor = (float)(data[7]);
}

void DM_Motor_Enable_Motor(DM_Motor_t *motor)
{
    CAN_Instance_t *motor_can_instance = motor->can_instance;
    uint8_t *data = motor_can_instance->tx_buffer;
    data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFC;
    CAN_Transmit(motor_can_instance);
}

void DM_Motor_Disable_Motor(DM_Motor_t *motor)
{
    CAN_Instance_t *motor_can_instance = motor->can_instance;
    uint8_t *data = motor_can_instance->tx_buffer;
    data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFD;
    CAN_Transmit(motor_can_instance);
}

void DM_Motor_Ctrl_MIT(DM_Motor_t *motor, float target_pos, float target_vel, float torq)
{
    uint16_t pos_temp, vel_temp, kp_temp, kd_temp, torq_temp;
    CAN_Instance_t *motor_can_instance = motor->can_instance;
    uint8_t *data = motor_can_instance->tx_buffer;
    motor->target_pos = target_pos;
    motor->target_vel = target_vel;
    motor->torq = torq;
    pos_temp = float_to_uint(motor->target_pos, P_MIN, P_MAX, 16);
    vel_temp = float_to_uint(motor->target_vel, V_MIN, V_MAX, 12);
    kp_temp = float_to_uint(motor->kp, KP_MIN, KP_MAX, 12);
    kd_temp = float_to_uint(motor->kd, KD_MIN, KD_MAX, 12);
    torq_temp = float_to_uint(motor->torq, T_MIN, T_MAX, 12);

    data[0] = (pos_temp >> 8);
    data[1] = pos_temp;
    data[2] = (vel_temp >> 4);
    data[3] = ((vel_temp & 0xF) << 4) | (kp_temp >> 8);
    data[4] = kp_temp;
    data[5] = (kd_temp >> 4);
    data[6] = ((kd_temp & 0xF) << 4) | (torq_temp >> 8);
    data[7] = torq_temp;

    CAN_Transmit(motor_can_instance);
}

void DM_Motor_Set_MIT_PD(DM_Motor_t *motor, float kp, float kd)
{
    motor->kp = kp;
    motor->kd = kd;
}

DM_Motor_t* DM_Motor_Init(DM_Motor_Config_t *config)
{
    DM_Motor_t *motor = malloc(sizeof(DM_Motor_t));
    motor->can_bus = config->can_bus;
    motor->control_mode = config->control_mode;
    motor->tx_id = config->tx_id;
    motor->rx_id = config->rx_id;
    motor->target_pos = 0.0f;
    motor->target_vel = 0.0f;
    motor->kp = config->kp;
    motor->kd = config->kd;
    motor->torq = 0.0f;
    motor->stats = calloc(sizeof(DM_Motor_Stats_t), 1);
    
    motor->can_instance = CAN_Device_Register(motor->can_bus, motor->tx_id, motor->rx_id, DM_Motor_Decode);
    motor->can_instance->binding_motor_stats =(void*) motor->stats;

    for (int i = 0; i < DM_MAX_DEVICE; i++)
    {
        if (g_dm_motors[i] == NULL)
        {
            g_dm_motors[i] = motor;
            break;
        }
    }
    return motor;
}



void DM_Motor_CtrlPosVel()
{
    //TODO:
}

void DM_Motor_CtrlVel()
{
    // TODO:
}
