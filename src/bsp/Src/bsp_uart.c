#include "bsp_uart.h"

//#include "referee_system.h"
#include "jetson_orin.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // if (huart->Instance == &huart6)
    // {
    //         Referee_Get_Data(REFEREE_BUFFER_LEN);
    // }

    if (huart->Instance == huart1.Instance)
    {
        Jetson_Orin_Decode();
    }
}
