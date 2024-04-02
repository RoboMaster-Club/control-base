#ifndef BSP_UART_H
#define BSP_UART_H

#include "usart.h"

typedef struct _UART_Instance
{
    UART_HandleTypeDef *huart;
    uint8_t *rx_buffer;
    uint8_t *tx_buffer;
    uint8_t rx_size;
    uint8_t tx_size;
    void (*uart_rx_callback)(struct _UART_Instance *);
} UART_Instance_t;

typedef struct _UART_Config_t
{
    UART_HandleTypeDef *huart; // UART_HandleTypeDef
    uint8_t *rx_buffer;
    uint8_t *tx_buffer;
    uint8_t tx_size; // byte
    uint8_t rx_size; // byte
    uint8_t mode; // 0: Normal, 1: Interrupt, 2: DMA
    void (*uart_rx_callback)(struct _UART_Instance *);
} UART_Config_t;

#endif // BSP_UART_H