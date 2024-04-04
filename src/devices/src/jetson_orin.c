#include "jetson_orin.h"

#include <string.h>

#include "imu_task.h"
#include "bsp_daemon.h"

Jetson_Orin_Data_t g_orin_data;

UART_Instance_t *g_orin_uart_instance_ptr;
Daemon_Instance_t *g_orin_daemon_instance_ptr;

void Jetson_Orin_Rx_Callback(UART_Instance_t *uart_instance)
{
	UNUSED(uart_instance);
	// TODO: update buffer reference from uart_instance
	if(g_orin_data.rx_buffer[0] == 0xAA)
	{
		// If matching frame header, reload Daemon
		Daemon_Reload(g_orin_daemon_instance_ptr);

		// Decode data
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

/**
 * @brief Callback function for timeout of Jetson Orin used by Daemon
 * @details When not receiving data from Orin for a certain time, will try 
 * 			to reinitialize the UART service. Daemon is reloaded to prevent
 * 			continuous reinitialization at high frequency. 
 * @param void
*/
void Jetson_Orin_Timeout_Callback()
{
	// Attemp to reinitialize UART service
	UART_Service_Init(g_orin_uart_instance_ptr);
	
	// Reload Daemon to prevent continuous reinitialization
	Daemon_Reload(g_orin_daemon_instance_ptr);
}

void Jetson_Orin_Init(UART_HandleTypeDef *huartx)
{
	// register UART instance
	g_orin_uart_instance_ptr = UART_Register(huartx, g_orin_data.rx_buffer, ORIN_DATA_RX_BUFER_SIZE, Jetson_Orin_Rx_Callback);
	
	// register Daemon instance
	// timeout is defined in the header file
	uint16_t reload_value = ORIN_TIMEOUT_MS / DAEMON_PERIOD;
	uint16_t initial_counter = reload_value;
	g_orin_daemon_instance_ptr = Daemon_Register(reload_value, initial_counter, Jetson_Orin_Timeout_Callback);
}

void Jetson_Orin_Send_Data(void)
{
	// update data to be sent
	g_orin_data.sending.pitch_angle = g_imu.rad.pitch;
	g_orin_data.sending.pitch_angular_rate = g_imu.bmi088_raw.gyro[1];
	g_orin_data.sending.yaw_angular_rate = g_imu.bmi088_raw.gyro[2];
	g_orin_data.sending.position_x = 0;
	g_orin_data.sending.position_y = 0;
	g_orin_data.sending.orientation = g_imu.rad.yaw;
	g_orin_data.sending.velocity_x = 0;
	g_orin_data.sending.velocity_y = 0;
	
	// float to byte conversion
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
	
	//UART_Transmit(g_orin_uart_instance_ptr, g_orin_data.tx_buffer, sizeof(g_orin_data.tx_buffer), UART_DMA);
}