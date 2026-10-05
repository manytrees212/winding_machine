#include <stm32f1xx_hal.h>
#include <logger.h>

#include "display_st7789.h"
#include "gpio.h"
#include "spi_init.h"
#include "tim.h"

void hal_spi_transmit(const uint8_t* data, uint16_t len){
    HAL_SPI_Transmit(&hspi1, data, len, 500);
}

void hal_set_cs_pin(bool state){
    if(state) return; //there is no CS pin
}

void hal_set_dc_pin(bool state){
    if(state){
        HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
    }else{
        HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
    }
}

void hal_set_rst_pin(){
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, 0);
    HAL_Delay(10);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, 1);
    HAL_Delay(150);
}

void hal_set_blk_pin(uint8_t brightness_percent){
    (void)brightness_percent;
#if 0
    if(brightness_percent>100){
        brightness_percent=100;
    }
    uint16_t brightness = brightness_percent*10;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, brightness);
#endif
}

void hal_set_delay(uint16_t ms){
    HAL_Delay(ms);
}
