#ifndef UART1_INIT_H
#define UART1_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f1xx_hal.h>
#include <stdint.h>

#define RX_BUF_SIZE 64

extern UART_HandleTypeDef huart1;
extern volatile uint16_t rx_head;
extern volatile uint16_t rx_tail;
extern uint8_t rx_data;

void UART1_Init();
void StartReceiveUART1();
uint8_t UART1_Data_Available();
uint8_t UART1_Read();

#ifdef __cplusplus
}
#endif

#endif //UART1_INIT_H
