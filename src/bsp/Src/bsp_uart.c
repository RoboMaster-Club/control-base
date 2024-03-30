#include "bsp_uart.h"

#include "referee_system.h"
#include "jetson_orin.h"

/**
 * @brief  This is the callback function for the UART receive interrupt.
 * This function overrides the weak implementation in stm32f4xx_hal_uart.c.
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // Check which UART is calling the callback
    if (huart == Referee_System.huart) {
        // Decode Referee system data
        Referee_Get_Data();
    }
    if (huart == g_orin_data.huartx)
    {
        // Decode Jetson Orin data
        Jetson_Orin_Decode();
    }
}
