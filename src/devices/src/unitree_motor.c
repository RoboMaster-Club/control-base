#pragma message "TODO: add transmission reduction ratio support, otherwise, the all parameters are for the motor rotor side."
#pragma message "TODO: add multiple uart support"
#include "unitree_motor.h"

#include "bsp_daemon.h"
#include "user_math.h"
#include <stdlib.h>

#define UNITREE_MOTOR_NUM (4)

/******** TODO: add multiple uart support *******/
Unitree_Motor_t *g_unitree_motors_ptr[UNITREE_MOTOR_NUM];
Daemon_Instance_t *g_unitree_motor_daemon_ptr;
UART_Instance_t *g_unitree_motor_uart_instance_ptr;
/******** TODO: add multiple uart support *******/

uint8_t g_unitree_motor_num = 0;
uint8_t g_unitree_motor_service_initialzed = 0;
uint8_t g_unitree_motor_statsx_buffer[sizeof(Unitree_Motor_Rx_Buffer_t)];

/* Pricate Function and Callback Function Prototype */
void Unitree_Motor_Callback(UART_Instance_t *uart_instance);
void Unitree_Motor_Timeout_Callback(void);
int _Unitree_Motor_Decode(Unitree_Motor_Rx_Buffer_t *motor_data);
int _Unitree_Motor_Transform_Cmd(Unitree_Motor_Cmd_Package_t *cmd);

/* Function Definition */

/**
 * @brief Initialize Unitree Motor Motor
 * @details Initialize uart service and daemon protection if this is
 * the first motor to be initialized on this uart instance.
 *
 * @param huartx UART_HandleTypeDef instance for the motor
 * @param motor_id Motor ID (0-14)
 *
 * @return Unitree_Motor_t* Motor instance
 */
Unitree_Motor_t *Unitree_Motor_Init(UART_HandleTypeDef *huartx, uint8_t motor_id, float k_pos, float k_vel)
{
    if (g_unitree_motor_service_initialzed == 0)
    {
        g_unitree_motor_uart_instance_ptr = UART_Register(huartx, g_unitree_motor_statsx_buffer,
                                                          sizeof(g_unitree_motor_statsx_buffer), Unitree_Motor_Callback);
        uint16_t reload_value = UNITREE_TIMEOUT_MS / DAEMON_PERIOD;
        uint16_t initial_counter = reload_value;
        g_unitree_motor_daemon_ptr = Daemon_Register(reload_value, initial_counter, Unitree_Motor_Timeout_Callback);
    }
    Unitree_Motor_t *motor = (Unitree_Motor_t *)malloc(sizeof(Unitree_Motor_t));
    motor->motor_id = motor_id;
    motor->cmd.id = motor_id;
    motor->cmd.mode = UNITREE_IDLE_MODE;
    motor->cmd.k_pos = k_pos;
    motor->cmd.k_vel = k_vel;
    motor->uart_instance = g_unitree_motor_uart_instance_ptr;
    
    g_unitree_motors_ptr[g_unitree_motor_num++] = motor;
    return motor;
}

/**
 * @brief Set torque to the motor. Will use the last FOC control parameters.
 * or the default FOC control parameters when initialized.
 * 
 * @param motor Motor instance
 * @param torque Desired joint output torque (Nm)
 * 
*/
HAL_StatusTypeDef Unitree_Motor_Set_Torque(Unitree_Motor_t *motor, float torque)
{
    motor->cmd.torque = torque;
    _Unitree_Motor_Transform_Cmd(&motor->cmd);
    return UART_Transmit(motor->uart_instance, (uint8_t *)&motor->cmd, sizeof(motor->cmd), UART_DMA);
}

/**
 * @brief Set the FOC control parameters.
 * The actual torque command given to FOC is:
 * k_pos*delta_pos + k_vel*delta_velocity + torque
 * 
 * @param motor Motor instance
 * @param k_pos Joint stiffness coefficient
 * @param k_vel Joint velocity coefficient
 * @param position Desired joint position (rad)
 * @param velocity Desired joint velocity (rad/s)
 * @param torque Desired joint output torque (Nm)
 * 
*/
HAL_StatusTypeDef Unitree_Motor_FOC(Unitree_Motor_t *motor, float k_pos, float k_vel, float position, float velocity, float torque)
{
    motor->cmd.k_pos = k_pos;
    motor->cmd.k_vel = k_vel;
    motor->cmd.position = position;
    motor->cmd.velocity = velocity;
    motor->cmd.torque = torque;
    _Unitree_Motor_Transform_Cmd(&motor->cmd);
    return UART_Transmit(motor->uart_instance, (uint8_t *)&motor->cmd, sizeof(motor->cmd), UART_DMA);
}

int _Unitree_Motor_Transform_Cmd(Unitree_Motor_Cmd_Package_t *cmd)
{
    cmd->hex_len = 17; // TODO: might be redundant
    cmd->tx_buffer.head[0] = 0xFE;
    cmd->tx_buffer.head[1] = 0xEE;

    /** TODO: Change this to error triggering **/
    // __MAX_LIMIT(cmd->id, 0, 15);
    // __MAX_LIMIT(cmd->mode, 0, 7);
    __MAX_LIMIT(cmd->k_pos, 0.0f, 25.599f);
    __MAX_LIMIT(cmd->k_vel, 0.0f, 25.599f);
    __MAX_LIMIT(cmd->torque, -127.99f, 127.99f);
    __MAX_LIMIT(cmd->velocity, -804.00f, 804.00f);
    __MAX_LIMIT(cmd->position, -411774.0f, 411774.0f);

    cmd->tx_buffer.id_mode.id = cmd->id;
    cmd->tx_buffer.id_mode.mode = cmd->mode;
    cmd->tx_buffer.cmd.k_pos = cmd->k_pos / 25.6f * 32768;
    cmd->tx_buffer.cmd.k_spd = cmd->k_vel / 25.6f * 32768;
    cmd->tx_buffer.cmd.pos_des = cmd->position / 6.2832f * 32768;
    cmd->tx_buffer.cmd.spd_des = cmd->velocity / 6.2832f * 256;
    cmd->tx_buffer.cmd.tor_des = cmd->torque * 256;
    cmd->tx_buffer.CRC16 = crc_ccitt(0, (uint8_t *)&cmd->tx_buffer, 15);
    return 0;
}

int _Unitree_Motor_Decode(Unitree_Motor_Rx_Buffer_t *motor_data)
{
    if (motor_data->CRC16 !=
        crc_ccitt(0, (uint8_t *)&motor_data, 14))
    {
        /******* TODO: Add CRC Error Log ******/
        return 1;
    }

    for (int i = 0; i < g_unitree_motor_num; i++)
    {
        if (g_unitree_motors_ptr[i]->motor_id == motor_data->id_mode.id)
        {
            Unitree_Motor_Stats_t *motor_stats = &(g_unitree_motors_ptr[i]->stats);
            motor_stats->mode = motor_data->id_mode.mode;
            motor_stats->Temp = motor_data->feedback.temp;
            motor_stats->error_code = motor_data->feedback.error_code;
            motor_stats->velocity = ((float)motor_data->feedback.speed / 256) * 6.2832f;
            motor_stats->torque = ((float)motor_data->feedback.torque) / 256;
            motor_stats->position = 6.2832f * ((float)motor_data->feedback.position) / 32768;
            motor_stats->foot_force = motor_data->feedback.force;
            return 0;
            break;
        }
    }
    // If reach here, motor ID is not found
    // TODO: Add Motor ID Error Log
    return 1;
}

void Unitree_Motor_Callback(UART_Instance_t *uart_instance)
{
    uint8_t *motor_rx_buffer = uart_instance->rx_buffer;
    // Check Header
    if (motor_rx_buffer[0] == 0xFE && motor_rx_buffer[1] == 0xEE)
    {
        _Unitree_Motor_Decode((Unitree_Motor_Rx_Buffer_t *)motor_rx_buffer);
    }
}

void Unitree_Motor_Timeout_Callback()
{
    UART_Service_Init(g_unitree_motor_uart_instance_ptr);
    // TODO: Timeout Error Log
}