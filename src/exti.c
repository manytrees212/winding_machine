#include "exti.h"

#include <stm32f1xx_hal.h>

#include <logger.h>
#include <logger_with_hal.h>
#include <gpio.h>


static volatile uint32_t last_home_interrupt_time = 0;
static const uint32_t DEBOUNCE_TIME_MS = 50;

#if 0
static volatile uint32_t last_reset_interrupt_time = 0;

__weak void Flash_Reset_Button_Callback(){
    LOG_INFO("WeakFlash_Reset_Button_Callback");
}

void EXTI0_IRQHandler() {

    __HAL_GPIO_EXTI_CLEAR_IT(BUILD_IN_BUTTON);

    uint32_t current_time = HAL_GetTick();
    if ((current_time - last_reset_interrupt_time) > DEBOUNCE_TIME_MS) {
        if (HAL_GPIO_ReadPin(GPIOA, BUILD_IN_BUTTON) == GPIO_PIN_SET) {
            Flash_Reset_Button_Callback();
        }
    }
    last_reset_interrupt_time = current_time;
}
#endif

__weak void Feeder_Home_Position_Callback(){
    LOG_INFO("Weak Feeder_Home_Position_Callback");
}

void EXTI15_10_IRQHandler(){

    __HAL_GPIO_EXTI_CLEAR_IT(HOME_POS_PIN);

    uint32_t current_time = HAL_GetTick();
    if((current_time - last_home_interrupt_time) > DEBOUNCE_TIME_MS){
        if(HAL_GPIO_ReadPin(GPIOB, HOME_POS_PIN) == GPIO_PIN_RESET){
            LOG_INFO("HOME position reached");
            Feeder_Home_Position_Callback();
        }
    }
    last_home_interrupt_time = current_time;
}

void EXTI_Init() {
#if 0
    //for reset flash
    __HAL_GPIO_EXTI_CLEAR_IT(BUILD_IN_BUTTON);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);
#endif
    //for feeder home position
    __HAL_GPIO_EXTI_CLEAR_IT(HOME_POS_PIN);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
    LOG_INFO("EXTI READY");
}
