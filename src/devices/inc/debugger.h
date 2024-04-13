#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "bsp_uart.h"
#include "bsp_daemon.h"
#include "bsp_serial.h"

#define DEBUGGER_DATA_RX_BUFER_SIZE (1)
#define DEBUGGER_DATA_TX_BUFER_SIZE (1024)

#define DEBUGGER_TIMEOUT_MS (10000)
#define DEBUGGER_PERIOD (100)

typedef struct {
    uint8_t rx_buffer[DEBUGGER_DATA_RX_BUFER_SIZE];
    uint8_t tx_buffer[DEBUGGER_DATA_TX_BUFER_SIZE];
} Debugger_Data_t;

extern Debugger_Data_t g_debugger_data;
void Debugger_Init(UART_HandleTypeDef *huartx);
void Debugger_Log_Data(const char *data, ...);

#endif