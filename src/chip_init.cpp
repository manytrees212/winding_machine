#include "chip_init.hpp"

#include <stm32f1xx_hal.h>
#include <logger.h>
#include <logger_with_hal.h>
#include <error_handler.h>


#include "sysclock_init.h"
#include "gpio.h"
#include "exti.h"
#include "uart_init.h"
#include "spi_init.h"
#include "tim.h"

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
}

void LED_On_1_sec(){
    LED_On();
    HAL_Delay(1000);
    LED_Off();
}

void Chip_Init(){
    HAL_Init();

    SystemClock_Init();
    GPIO_Init();
    UART1_Init();

    HAL_Logger_Init();
    LOG_INFO("System starting...");
    LOG_DEBUG("Debug messages enabled");
    LED_On_1_sec();

    EXTI_Init();
    SPI_Init();
    TIM1_Init();
    TIM2_Init();
    TIM3_Init();
    TIM4_Init();

    LOG_INFO("Chip is INIT");
}
