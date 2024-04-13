#include "debugger.h"
#include "stdarg.h"

Debugger_Data_t g_debugger_data;

UART_Instance_t *g_debugger_uart_instance_ptr;
Daemon_Instance_t *g_debugger_daemon_instance_ptr;

static uint8_t debugger_init = 0;

void Debugger_Rx_Callback(UART_Instance_t *uart_instance)
{
    return;
}

void Debugger_Timeout_Callback()
{
    return;
}

void Debugger_Init(UART_HandleTypeDef *huartx)
{
    debugger_init = 1;
    // register UART instance
    g_debugger_uart_instance_ptr = UART_Register(huartx, g_debugger_data.rx_buffer, DEBUGGER_DATA_RX_BUFER_SIZE, Debugger_Rx_Callback);

    // register Daemon instance
    // timeout is defined in the header file
    uint16_t reload_value = DEBUGGER_TIMEOUT_MS / DAEMON_PERIOD;
    uint16_t initial_counter = reload_value;
    g_debugger_daemon_instance_ptr = Daemon_Register(reload_value, initial_counter, Debugger_Timeout_Callback);
    return;
}

void Debugger_Log_Data(const char *data, ...)
{
    if (debugger_init)
    {
        va_list args;
        va_start(args, data);
        DEBUG_PRINTF(g_debugger_uart_instance_ptr->uart_handle, data, args);
        va_end(args);
    }
}
