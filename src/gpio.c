#include "gpio.h"
#include <stm32f1xx_hal.h>

void GPIO_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*==============================*/
    /*          CLOCK               */
    /*==============================*/
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    /*==============================*/
    /*          REMAP               */
    /*==============================*/
    // Remap JTAG pins to GPIO
    __HAL_AFIO_REMAP_SWJ_NOJTAG();

    // Remap Timer3 CH1 and CH2 for encoder
    __HAL_AFIO_REMAP_TIM3_PARTIAL();

    /*==============================*/
    /*          LED in-build        */
    /*==============================*/
    // PB2, for debug
    GPIO_InitStruct.Pin = BUILD_IN_LED_PB2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, BUILD_IN_LED_PB2, GPIO_PIN_RESET);

    /*==============================*/
    /*          UART1               */
    /*==============================*/
    // TX(PA9): Alternate Function Push-Pull
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    //UART1 RX(PA10): Input Floating
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*================================*/
    /*          Display_st7789        */
    /*================================*/
    // DC(PA2) RES(PA3)
    GPIO_InitStruct.Pin = DC_Pin | RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //BLK (PA4) backlight connected to 3V3
#if 0
    GPIO_InitStruct.Pin = BLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

    //SPI1 SCK(PA5), MOSI(PA7)
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, BLK_Pin, GPIO_PIN_SET);// Turn backlight ON
    HAL_GPIO_WritePin(GPIOA, DC_Pin, GPIO_PIN_SET); // DC high (data mode)
    HAL_GPIO_WritePin(GPIOA, RST_Pin, GPIO_PIN_SET);// RST high (not reset)

    /*================================*/
    /*         Button panel           */
    /*================================*/
    //Matrix like arrange

    GPIO_InitStruct.Pin = ROW1_PIN | ROW2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, ROW1_PIN | ROW2_PIN, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = COL1_PIN | COL2_PIN | COL3_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*================================*/
    /*      Motor driver BTS7960      */
    /*================================*/
    //activating pins L_EN(PB10) and R_EN(PB11)
    GPIO_InitStruct.Pin = L_EN_PIN | R_EN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, L_EN_PIN | R_EN_PIN, GPIO_PIN_RESET);

    //rotor PWM: LPWM(PB6) RPWM(PB7)
    GPIO_InitStruct.Pin = LPWM_PIN | RPWM_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //current pins L_IS(PB0) and R_IS(PB1): ADC
    GPIO_InitStruct.Pin = L_IS_PIN | R_IS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*================================*/
    /*         Feeder driver          */
    /*================================*/
    //feeder PWM: IN1(PB13) and IN2(PB14)
    GPIO_InitStruct.Pin = IN1_PIN | IN2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*================================*/
    /* Opto sensor for home position  */
    /*================================*/
    GPIO_InitStruct.Pin = HOME_POS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    __HAL_GPIO_EXTI_CLEAR_IT(HOME_POS_PIN);

    /*================================*/
    /*       Motor encoder rpm        */
    /*================================*/
    //PA0 and PA1
    GPIO_InitStruct.Pin = MOTOR_ENCODER_CHANNEL_A | MOTOR_ENCODER_CHANNEL_B;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*================================*/
    /*       Feeder feeder rpm         */
    /*================================*/
    //PB4 and PB5
    GPIO_InitStruct.Pin = FEEDER_ENCODER_CHANNEL_A | FEEDER_ENCODER_CHANNEL_B;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void LED_On(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
}

void LED_Off(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
}

void GPIO_ToggleLED(void){
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
}
