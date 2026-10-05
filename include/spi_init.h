#ifndef SPI_INIT_H
#define SPI_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f1xx_hal.h>

extern SPI_HandleTypeDef hspi1;
void SPI_Init();

#ifdef __cplusplus
}
#endif

#endif //SPI_INIT_H
