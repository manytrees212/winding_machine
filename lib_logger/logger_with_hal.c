#include "logger.h"
#include "logger_with_hal.h"
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;

static void Uart_Output_Function(const char* str, size_t len){
    HAL_UART_Transmit(&huart1, (uint8_t*)str, len, 100);
}

void HAL_Logger_Init(void){
    Logger_Init(Uart_Output_Function);
}