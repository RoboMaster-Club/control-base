#include "jetson_orin.h"

#include <string.h>

#include "imu_task.h"
#include "bsp_uart.h"

Jetson_Orin_Data_t g_orin_data;

void Jetson_Orin_Decode(void)
{
	if(g_orin_data.rx_buffer[0] == 0xAA)
	{
		g_orin_data.receiving.frame_id = g_orin_data.rx_buffer[0];
		g_orin_data.receiving.frame_type = g_orin_data.rx_buffer[1];
		switch(g_orin_data.receiving.frame_type)
		{
			case 0:
				memcpy(&g_orin_data.receiving.float_byte.data[0],&g_orin_data.rx_buffer[4],8*sizeof(uint8_t));
				g_orin_data.receiving.auto_aiming.yaw = g_orin_data.receiving.float_byte.data[0];
				g_orin_data.receiving.auto_aiming.pitch = g_orin_data.receiving.float_byte.data[1];
				break;
			
			case 1:
				memcpy(&g_orin_data.receiving.float_byte.data[0],&g_orin_data.rx_buffer[4],12*sizeof(uint8_t));
				g_orin_data.receiving.navigation.x_vel = g_orin_data.receiving.float_byte.data[0];
				g_orin_data.receiving.navigation.y_vel = g_orin_data.receiving.float_byte.data[1];
				g_orin_data.receiving.navigation.yaw_angular_rate = g_orin_data.receiving.float_byte.data[2];
				g_orin_data.receiving.navigation.state = g_orin_data.rx_buffer[16];
				break;
			
			case 2:
				g_orin_data.receiving.heart_beat.a = g_orin_data.rx_buffer[2];
				g_orin_data.receiving.heart_beat.b = g_orin_data.rx_buffer[3];
				g_orin_data.receiving.heart_beat.c = g_orin_data.rx_buffer[4];
				g_orin_data.receiving.heart_beat.d = g_orin_data.rx_buffer[5];
				break;
			
			default:
				break;
		}
	}
}

void Jetson_Orin_Init(void)
{
	HAL_UART_Receive_DMA(&huart1, g_orin_data.rx_buffer, 20);
}

void Jetson_Orin_Send_Data(void)
{
	g_orin_data.sending.pitch_angle = g_imu.rad.pitch;
	g_orin_data.sending.pitch_angular_rate = g_imu.bmi088_raw.gyro[1];
	g_orin_data.sending.yaw_angular_rate = g_imu.bmi088_raw.gyro[2];
	g_orin_data.sending.position_x = 0;
	g_orin_data.sending.position_y = 0;
	g_orin_data.sending.orientation = g_imu.rad.yaw;
	g_orin_data.sending.velocity_x = 0;
	g_orin_data.sending.velocity_y = 0;
	
	g_orin_data.sending.float_byte.data[0] = g_orin_data.sending.pitch_angle;
	g_orin_data.sending.float_byte.data[1] = g_orin_data.sending.pitch_angular_rate;
	g_orin_data.sending.float_byte.data[2] = g_orin_data.sending.yaw_angular_rate;
	g_orin_data.sending.float_byte.data[3] = g_orin_data.sending.position_x;
	g_orin_data.sending.float_byte.data[4] = g_orin_data.sending.position_y;
	g_orin_data.sending.float_byte.data[5] = g_orin_data.sending.orientation;
	g_orin_data.sending.float_byte.data[6] = g_orin_data.sending.velocity_x;
	g_orin_data.sending.float_byte.data[7] = g_orin_data.sending.velocity_y;
	
	g_orin_data.tx_buffer[0] = 0xAA;
	memcpy(&g_orin_data.tx_buffer[1],&g_orin_data.sending.float_byte.data_bytes[0], 32*sizeof(uint8_t));
	
	HAL_UART_Transmit_DMA(&huart1, g_orin_data.tx_buffer, sizeof(g_orin_data.tx_buffer));
}