#include "jetson_orin.h"

#include <string.h>

#include "imu_task.h"
#include "bsp_uart.h"
#include "crc.h"

// #define CRC8_POLY 0x07 // Standard CRC-8 polynomial

// // Function to calculate CRC8
// uint8_t crc8(uint8_t *data, uint8_t len) {
//     uint8_t crc = 0x00;
//     while (len--) {
//         crc ^= *data++;
//         for (uint8_t i = 8; i; i--) {
//             crc = (crc & 0x80) ? (crc << 1) ^ CRC8_POLY : crc << 1;
//         }
//     }
//     return crc;
// }

Jetson_Orin_Data_t g_orin_data;
uint8_t g_orin_rx_buffer[JETSON_ORIN_RX_BUFFER_SIZE * 2] = {0}; // double buffer for alignment

void Jetson_Orin_Decode(void)
{
	static uint8_t rx_buffer_header_idx = 0;
	// check alignment and move buffer
	if (g_orin_rx_buffer[rx_buffer_header_idx] != 0xAA)
	{
		// if mismatch, check header index and go to case 1 without break
		for (uint8_t i = 0; i < JETSON_ORIN_RX_BUFFER_SIZE; i++)
		{
			if (g_orin_rx_buffer[i] == 0xAA)
			{
				rx_buffer_header_idx = i;
			}
		}
		// good luck if you can't find the header
	}
	// CRC check
	// uint8_t crc_check_result = crc8(&g_orin_rx_buffer[rx_buffer_header_idx], JETSON_ORIN_RX_BUFFER_SIZE - 4);

	// if (memcmp(&g_orin_rx_buffer[rx_buffer_header_idx + JETSON_ORIN_RX_BUFFER_SIZE - 4], &crc_check_result, 4) != 0)
	// {
	// 	// CRC check failed
	// 	memcpy(g_orin_rx_buffer, g_orin_rx_buffer + JETSON_ORIN_RX_BUFFER_SIZE, JETSON_ORIN_RX_BUFFER_SIZE * sizeof(uint8_t));
	// 	return;
	// }

	g_orin_data.receiving.frame_id = g_orin_rx_buffer[rx_buffer_header_idx + 0];
	g_orin_data.receiving.frame_type = g_orin_rx_buffer[rx_buffer_header_idx + 1];
	switch (g_orin_data.receiving.frame_type)
	{
	case 0:
		memcpy(&g_orin_data.receiving.float_byte.data[0], &g_orin_rx_buffer[rx_buffer_header_idx + 4], 8 * sizeof(uint8_t));
		g_orin_data.receiving.auto_aiming.yaw = g_orin_data.receiving.float_byte.data[0];
		g_orin_data.receiving.auto_aiming.pitch = g_orin_data.receiving.float_byte.data[1];
		break;

	case 1:
		memcpy(&g_orin_data.receiving.float_byte.data[0], &g_orin_rx_buffer[rx_buffer_header_idx + 4], 12 * sizeof(uint8_t));
		g_orin_data.receiving.navigation.x_vel = g_orin_data.receiving.float_byte.data[0];
		g_orin_data.receiving.navigation.y_vel = g_orin_data.receiving.float_byte.data[1];
		g_orin_data.receiving.navigation.yaw_angular_rate = g_orin_data.receiving.float_byte.data[2];
		g_orin_data.receiving.navigation.state = g_orin_rx_buffer[rx_buffer_header_idx + 16];
		break;

	case 2:
		g_orin_data.receiving.heart_beat.a = g_orin_rx_buffer[rx_buffer_header_idx + 2];
		g_orin_data.receiving.heart_beat.b = g_orin_rx_buffer[rx_buffer_header_idx + 3];
		g_orin_data.receiving.heart_beat.c = g_orin_rx_buffer[rx_buffer_header_idx + 4];
		g_orin_data.receiving.heart_beat.d = g_orin_rx_buffer[rx_buffer_header_idx + 5];
		break;

	default:
		break;
	}
	memcpy(g_orin_rx_buffer, g_orin_rx_buffer + JETSON_ORIN_RX_BUFFER_SIZE, JETSON_ORIN_RX_BUFFER_SIZE * sizeof(uint8_t));
}

void Jetson_Orin_Init(UART_HandleTypeDef *huartx)
{
	g_orin_data.huartx = huartx;
	HAL_UART_Receive_DMA(huartx, g_orin_rx_buffer + JETSON_ORIN_RX_BUFFER_SIZE, JETSON_ORIN_RX_BUFFER_SIZE);
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
	memcpy(&g_orin_data.tx_buffer[1], &g_orin_data.sending.float_byte.data_bytes[0], 32 * sizeof(uint8_t));

	HAL_UART_Transmit_DMA(g_orin_data.huartx, g_orin_data.tx_buffer, sizeof(g_orin_data.tx_buffer));
}