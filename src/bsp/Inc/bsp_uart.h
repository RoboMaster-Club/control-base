#ifndef BSP_UART_H
#define BSP_UART_H

#include "usart.h"

typedef struct _UART_Instance
{
    UART_HandleTypeDef *uart_handle;
    uint8_t *rx_buffer;
    uint16_t rx_buffer_size;
    void (*callback)(struct _UART_Instance *uart_instance);
} UART_Instance_t;

void UART_Service_Init(UART_Instance_t *uart_insatce);
UART_Instance_t *UART_Register(UART_HandleTypeDef *huart, uint8_t *rx_buffer, uint8_t rx_buffer_size, void (*callback)(UART_Instance_t *uart_instance)); 

#endif // BSP_UART_H