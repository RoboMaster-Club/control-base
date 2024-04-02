#include "unitree_motor.h"
#include "usart.h"
#include "bsp_uart.h"
#include "motor.h"

#define SATURATE(_IN, _MIN, _MAX) \
    {                             \
        if (_IN < _MIN)           \
            _IN = _MIN;           \
        else if (_IN > _MAX)      \
            _IN = _MAX;           \
    }
#define UNITREE_MOTOR_NUM 4
Unitree_Motor *g_unitree_motor[UNITREE_MOTOR_NUM];

int modify_data(MOTOR_send *motor_s)
{
    motor_s->hex_len = 17;
    motor_s->motor_send_data.head[0] = 0xFE;
    motor_s->motor_send_data.head[1] = 0xEE;

    //		SATURATE(motor_s->id,   0,    15);
    //		SATURATE(motor_s->mode, 0,    7);
    SATURATE(motor_s->K_P, 0.0f, 25.599f);
    SATURATE(motor_s->K_W, 0.0f, 25.599f);
    SATURATE(motor_s->T, -127.99f, 127.99f);
    SATURATE(motor_s->W, -804.00f, 804.00f);
    SATURATE(motor_s->Pos, -411774.0f, 411774.0f);

    motor_s->motor_send_data.mode.id = motor_s->id;
    motor_s->motor_send_data.mode.status = motor_s->mode;
    motor_s->motor_send_data.comd.k_pos = motor_s->K_P / 25.6f * 32768;
    motor_s->motor_send_data.comd.k_spd = motor_s->K_W / 25.6f * 32768;
    motor_s->motor_send_data.comd.pos_des = motor_s->Pos / 6.2832f * 32768;
    motor_s->motor_send_data.comd.spd_des = motor_s->W / 6.2832f * 256;
    motor_s->motor_send_data.comd.tor_des = motor_s->T * 256;
    motor_s->motor_send_data.CRC16 = crc_ccitt(0, (uint8_t *)&motor_s->motor_send_data, 15);
    return 0;
}

int Unitree_Motor_Decode(MOTOR_recv *motor_r)
{
    if (motor_r->motor_recv_data.CRC16 !=
        crc_ccitt(0, (uint8_t *)&motor_r->motor_recv_data, 14))
    {
        // printf("[WARNING] Receive data CRC error");
        motor_r->correct = 0;
        return motor_r->correct;
    }
    else
    {
        motor_r->motor_id = motor_r->motor_recv_data.mode.id;
        motor_r->mode = motor_r->motor_recv_data.mode.status;
        motor_r->Temp = motor_r->motor_recv_data.fbk.temp;
        motor_r->MError = motor_r->motor_recv_data.fbk.MError;
        motor_r->W = ((float)motor_r->motor_recv_data.fbk.speed / 256) * 6.2832f;
        motor_r->T = ((float)motor_r->motor_recv_data.fbk.torque) / 256;
        motor_r->Pos = 6.2832f * ((float)motor_r->motor_recv_data.fbk.pos) / 32768;
        motor_r->footForce = motor_r->motor_recv_data.fbk.force;
        motor_r->correct = 1;
        return motor_r->correct;
    }
}

HAL_StatusTypeDef SERVO_Send_recv(MOTOR_send *pData, MOTOR_recv *rData)
{
    uint16_t rxlen = 0;

    modify_data(pData);

    HAL_UART_Transmit(&huart1, (uint8_t *)pData, sizeof(pData->motor_send_data), 10);

    HAL_UARTEx_ReceiveToIdle(&huart1, (uint8_t *)rData, sizeof(rData->motor_recv_data), &rxlen, 10);

    if (rxlen == 0)

        return HAL_TIMEOUT;

    if (rxlen != sizeof(rData->motor_recv_data))
        return HAL_ERROR;

    uint8_t *rp = (uint8_t *)&rData->motor_recv_data;
    if (rp[0] == 0xFE && rp[1] == 0xEE)
    {
        rData->correct = 1;
        extract_data(rData);
        return HAL_OK;
    }

    return HAL_ERROR;
}

HAL_StatusTypeDef Unitree_Motor_Set_Torque(Unitree_Motor *motor, float torque)
{
    motor->motor_send.T = torque;
    modify_data(&motor->motor_send);
    return HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&motor->motor_send, sizeof(motor->motor_send));
}

void Unitree_Motor_Init(Unitree_Motor *motor, uint8_t uart, uint8_t motor_id)
{
    motor->uart = uart;
    motor->motor_id = motor_id;
    motor->motor_send.id = motor_id;
    motor->motor_send.mode = 10;
    motor->motor_send.K_P = 0.0f;
    motor->motor_send.K_W = 0.0f;
    motor->motor_send.Pos = 0.0f;
    motor->motor_send.W = 0.0f;
    motor->motor_send.T = 0.0f;
    motor->motor_send.Res.L = 0;
    motor->motor_send.send_time = 0;
    motor->motor_send.hex_len = 0;

    motor->motor_recv.motor_id = motor_id;
    motor->motor_recv.mode = 0;
    motor->motor_recv.Temp = 0;
    motor->motor_recv.MError = 0;
    motor->motor_recv.T = 0.0f;
    motor->motor_recv.W = 0.0f;
    motor->motor_recv.Pos = 0.0f;
    motor->motor_recv.footForce = 0;
    motor->motor_recv.resv_time = 0;
    motor->motor_recv.correct = 0;
    motor->motor_recv.hex_len = 0;

    for (int i = 0; i < UNITREE_MOTOR_NUM; i++)
    {
        if (g_unitree_motor[i] == NULL)
        {
            g_unitree_motor[i] = motor;
            break;
        }
    }
}

HAL_StatusTypeDef Unitree_Motor_FOC(Unitree_Motor *motor, float K_P, float K_W, float Pos, float W, float T)
{
    motor->motor_send.K_P = K_P;
    motor->motor_send.K_W = K_W;
    motor->motor_send.Pos = Pos;
    motor->motor_send.W = W;
    motor->motor_send.T = T;
    modify_data(&motor->motor_send);
    return HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&motor->motor_send, sizeof(motor->motor_send));
}

HAL_StatusTypeDef Unitree_Motor_FOC_Send(Unitree_Motor *motor)
{
    modify_data(&motor->motor_send);
    return HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&motor->motor_send, sizeof(motor->motor_send));
}

void Unitree_Motor_Callback(UART_Instance_t *uart_instance)
{
    uint8_t *rp = uart_instance->rx_buffer;
    if (rp[0] == 0xFE && rp[1] == 0xEE)
    {
        extract_data(rp);
        return HAL_OK;
    }

    return HAL_ERROR;

    extract_data(uart_instance->rx_buffer);

}