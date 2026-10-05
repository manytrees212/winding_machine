#include "spi_init.h"

#include <stm32f1xx_hal.h>
#include <logger.h>
#include <error_handler.h>

SPI_HandleTypeDef hspi1;

void SPI_Init(){

    __HAL_RCC_SPI1_CLK_ENABLE();

    hspi1.Instance=SPI1;
    hspi1.Init.Mode=SPI_MODE_MASTER;
    hspi1.Init.Direction=SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize=SPI_DATASIZE_8BIT;
    //MODE3
    hspi1.Init.CLKPolarity=SPI_POLARITY_HIGH;
    hspi1.Init.CLKPhase=SPI_PHASE_2EDGE;
#if 0 //MODE0
    hspi1.Init.CLKPolarity=SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase=SPI_PHASE_1EDGE;
#endif
    hspi1.Init.NSS=SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_16; //~2.25MHz @36MHz
    hspi1.Init.FirstBit=SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode=SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial=7;

    if (HAL_SPI_Init(&hspi1) != HAL_OK){
        LOG_ERROR("SPI1 initialization failed!");
        ERROR_REPORT(spi_init_fail);
        Error_Handler();
    }
    __HAL_SPI_ENABLE(&hspi1);
    LOG_INFO("SPI1 READY");
}
