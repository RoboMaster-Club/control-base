#include "bsp_uart.h"

#include "referee_system.h"
#include "jetson_orin.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == Referee_System.huart) {
        Referee_Get_Data();
    }
    if (huart == g_orin_data.huartx)
    {
        Jetson_Orin_Decode();
    }
}
