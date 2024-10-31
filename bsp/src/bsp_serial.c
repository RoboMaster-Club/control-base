#include "bsp_serial.h"

#include "usart.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define MAX_STRING_LENGTH 1028

#ifdef DEBUG
#define DEBUG_PRINTF(huart, format, ...) debug_printf((huart), (format), ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(huart, format, ...) ((void)0)
#endif
char buffer[MAX_STRING_LENGTH];

/**
 * @brief Non-blocking printf function using DMA
*/
void debug_printf(UART_HandleTypeDef *huart, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    // Use vsnprintf instead of vsprintf to avoid buffer overflow
    vsnprintf(buffer, MAX_STRING_LENGTH, format, args);
    va_end(args);
    
    // Send the string over UART using DMA
    HAL_UART_Transmit_DMA(huart, (uint8_t*)buffer, strlen(buffer));
    while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET) {
            // Optionally, insert a timeout mechanism here to break from the loop if needed
    }
}
