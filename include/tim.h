#ifndef TIM_H
#define TIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f1xx_hal.h>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

void TIM1_Init(); //for feeder driver
void TIM2_Init(); //for motor rpm encoder
void TIM3_Init(); //for feeder rpm encoder
void TIM4_Init(); //for motor driver

#ifdef __cplusplus
}
#endif


#endif //TIM_H
