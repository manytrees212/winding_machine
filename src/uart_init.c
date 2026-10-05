#include "uart_init.h"

#include <stm32f1xx_hal.h>
#include <error_handler.h>


static uint8_t rx_buffer[RX_BUF_SIZE]={0};
volatile uint16_t rx_head=0;
volatile uint16_t rx_tail=0;
uint8_t rx_data=0;

UART_HandleTypeDef huart1;

void UART1_Init(){
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if(HAL_UART_Init(&huart1)!=HAL_OK){
        ERROR_SIGNAL(uart1_init_fail);
    }
    StartReceiveUART1();
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart){
    if(huart->Instance == USART1){
        __HAL_RCC_USART1_CLK_ENABLE();
        HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
}

void USART1_IRQHandler(void){
    HAL_UART_IRQHandler(&huart1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart->Instance == USART1){
        uint16_t next = (rx_head + 1) % RX_BUF_SIZE;
        if (next != rx_tail){
            rx_buffer[rx_head] = rx_data;
            rx_head = next;
        }
        StartReceiveUART1();
    }
}

void StartReceiveUART1(){
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
}

uint8_t UART1_Data_Available(){
    return (rx_head != rx_tail);
}

uint8_t UART1_Read(){
    if (rx_head == rx_tail){
        return 0;
    }
    uint8_t data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
    return data;
}
