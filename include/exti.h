#ifndef EXTI_H
#define EXTI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void EXTI_Init();
void EXTI15_10_IRQHandler();
void Feeder_Home_Position_Callback();

#ifdef __cplusplus
}
#endif

#endif //EXTI_H