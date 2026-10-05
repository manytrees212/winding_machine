#include "tim.h"

#include <stm32f1xx_hal.h>
#include <logger.h>
#include <error_handler.h>

#define FULL_CNT    0xffff
#define MIDDLE_CNT  0x7fff

TIM_HandleTypeDef htim1 = {0};
TIM_HandleTypeDef htim2 = {0};
TIM_HandleTypeDef htim3 = {0};
TIM_HandleTypeDef htim4 = {0};

void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim) {
    // Empty - not using break function
    (void)htim;
}

void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim) {
    // Empty - not using commutation function
    (void)htim;
}

/*for driver DRV8871 (feeder)*/
void TIM1_Init(){

    __HAL_RCC_TIM1_CLK_ENABLE(); //72 MHz (APB2)

    //base settings
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;   // 72 Mhz
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 4095;     // 72Mhz/4096 = 15.578 Hz
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if(HAL_TIM_PWM_Init(&htim1)!=HAL_OK){
        LOG_ERROR("TIM1 in PWM mode FAILED");
        Error_Handler();
    }

    //PWM for channel 1(IN1, PB13)
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;         // Clear on match, set on reload
    sConfigOC.Pulse = 0;                        // Default 0% of duty cycle
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; // Higher pulse higher speed
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        LOG_ERROR("TIM1 in PWM mode FAILED");
        Error_Handler();
    }

    //PWM for channel 2(IN2, PB14)
    sConfigOC.Pulse = 0;  // Start with 0% duty cycle
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
        LOG_ERROR("TIM1 in PWM mode FAILED");
        Error_Handler();
    }

    // UNLOCK ADVANCED TIM1 OUTPUTS
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;

    // Main Output Enable (MOE) register!
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK) {
        LOG_ERROR("TIM1 BDTR Config FAILED");
        Error_Handler();
    }

    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    LOG_INFO("TIM1 CH1 and CH2 in PWM mode READY");
}

/*for motor encoder*/
void TIM2_Init(){
    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance=TIM2;
    htim2.Init.Prescaler = 0; // 72MHz
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period=FULL_CNT-1;

    TIM_Encoder_InitTypeDef sEncoderConfig = {0};
    sEncoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;

    sEncoderConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    sEncoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sEncoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sEncoderConfig.IC1Filter = 0x0F;

    sEncoderConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sEncoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sEncoderConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sEncoderConfig.IC2Filter = 0x0F;

    if(HAL_TIM_Encoder_Init(&htim2, &sEncoderConfig) != HAL_OK){
        LOG_ERROR("TIM2 in Encoder mode FAILED");
        Error_Handler();
    }
    __HAL_TIM_SET_COUNTER(&htim2, MIDDLE_CNT);
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    LOG_INFO("TIM2 CH1 and CH2 in Encoder mode READY");
}

/*for feeder encoder*/
void TIM3_Init(){
    __HAL_RCC_TIM3_CLK_ENABLE();

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 0;    // 72MHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = FULL_CNT-1;

    TIM_Encoder_InitTypeDef sEncoderConfig = {0};
    sEncoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;

    sEncoderConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    sEncoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sEncoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sEncoderConfig.IC1Filter = 0x0F;

    sEncoderConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sEncoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sEncoderConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sEncoderConfig.IC2Filter = 0x0F;

    if(HAL_TIM_Encoder_Init(&htim3, &sEncoderConfig) != HAL_OK){
        LOG_ERROR("TIM3 in Encoder mode FAILED");
        Error_Handler();
    }

    __HAL_TIM_SET_COUNTER(&htim3, MIDDLE_CNT);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    LOG_INFO("TIM3 CH1 and CH2 in Encoder mode READY");
}

/*for driver BTS7960 (motor)*/
void TIM4_Init(){
    __HAL_RCC_TIM4_CLK_ENABLE();

    //base settings
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 0;   // 36 Mhz
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 2047;     // 36Mhz/2048 = 17.578 Hz
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.RepetitionCounter = 0;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if(HAL_TIM_PWM_Init(&htim4)!=HAL_OK){
        LOG_ERROR("TIM4 in PWM mode FAILED");
        Error_Handler();
    }

    //PWM for channel 1(RPWM, PB6)
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        LOG_ERROR("TIM4 in PWM mode FAILED");
        Error_Handler();
    }

    //PWM for channel 2(LPWM, PB7)
    sConfigOC.Pulse = 0;
    if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
        LOG_ERROR("TIM4 in PWM mode FAILED");
        Error_Handler();
    }

    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
    LOG_INFO("TIM4 CH1 and CH2 in PWM mode READY");
}