#include "bsp_uart.h"

#include "referee_system.h"
#include "jetson_orin.h"

#define UART_MAX_DEVICE 2
UART_Instance_t g_uart_instances[UART_MAX_DEVICE] = {0};

void UART_Register(UART_Config_t config) {
    for (uint8_t i = 0; i < UART_MAX_DEVICE; i++) {
        if (g_uart_instances[i].huart == NULL) {
            g_uart_instances[i].huart = config.huart;
            g_uart_instances[i].uart_rx_callback = config.uart_rx_callback;
            g_uart_instances[i].rx_size = config.rx_size;
            g_uart_instances[i].tx_size = config.tx_size;
            g_uart_instances[i].rx_buffer = config.rx_buffer;
            g_uart_instances[i].tx_buffer = config.tx_buffer;
            break;
        }
    }
}

/**
 * @brief  This is the callback function for the UART receive interrupt.
 * This function overrides the weak implementation in stm32f4xx_hal_uart.c.
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // // Check which UART is calling the callback
    // if (huart == Referee_System.huart) {
    //     // Decode Referee system data
    //     Referee_Get_Data();
    // }
    // if (huart == g_orin_data.huartx)
    // {
    //     // Decode Jetson Orin data
    //     Jetson_Orin_Decode();
    // }
    for (uint8_t i = 0; i < UART_MAX_DEVICE; i++) {
        if (huart == g_uart_instances[i].huart) {
            // Call the user defined callback function
            g_uart_instances[i].uart_rx_callback(&g_uart_instances[i]);
        }
    }

}
