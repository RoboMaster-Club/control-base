#include "bsp_uart.h"

#include <stdlib.h>
#include "memory.h"

#define UART_INSTANCE_MAX 3
UART_Instance_t *g_uart_insatnces[UART_INSTANCE_MAX];
uint8_t g_uart_instance_count = 0;

void UART_Service_Init(UART_Instance_t *uart_insatce)
{
    HAL_UARTEx_ReceiveToIdle_DMA(uart_insatce->uart_handle, uart_insatce->rx_buffer, uart_insatce->rx_buffer_size);
    __HAL_DMA_DISABLE_IT(uart_insatce->uart_handle->hdmarx, DMA_IT_HT); // disable half transfer interrupt
}

UART_Instance_t *UART_Register(UART_HandleTypeDef *huart, uint8_t *rx_buffer, uint8_t rx_buffer_size, void (*callback)(UART_Instance_t *uart_instance))
{
    UART_Instance_t *uart_instance = (UART_Instance_t *)malloc(sizeof(UART_Instance_t));
    uart_instance->uart_handle = huart;
    uart_instance->rx_buffer = rx_buffer;
    uart_instance->rx_buffer_size = rx_buffer_size;
    uart_instance->callback = callback;

    UART_Service_Init(uart_instance);
    
    g_uart_insatnces[g_uart_instance_count++] = uart_instance;
    return uart_instance;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    for (int i = 0; i < g_uart_instance_count; i++)
    {
        if (g_uart_insatnces[i]->uart_handle == huart)
        {
            if (g_uart_insatnces[i]->callback != NULL)
            {
                g_uart_insatnces[i]->callback(g_uart_insatnces[i]);

                HAL_UARTEx_ReceiveToIdle_DMA(huart, g_uart_insatnces[i]->rx_buffer, g_uart_insatnces[i]->rx_buffer_size);
                __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
            }
        }
    }
    
}