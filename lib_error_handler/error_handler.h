#ifndef ERROR_H
#define ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum{
    error_none = 0,
    uart_init_fail,
    spi_init_fail,
    spi_transmit_fail,
    display_init_fail,
    display_comm_fail,
    assert_fail,
    iwdg_fail,
    eeprom_fail,
    pointer_init_fail
}ErrorCode_t;

void Error_Handler();

void Error_Handler_withCode(ErrorCode_t error_code, const char* file, uint32_t line);
#define ERROR_REPORT(code) Error_Handler_withCode(code, __FILE__, __LINE__)

// For assert_failed (HAL debug)
void assert_failed(uint8_t *file, uint32_t line);


// ===== error handler to signal issue by LED(PB2) untill UART is initialized =====
typedef enum{
    clocks_fail = 3,
    uart1_init_fail = 5
}EarlyErrorCode_t;

void Error_Led_Init();
void Error_Blinking();
void Early_Error_Handler(EarlyErrorCode_t early_error);

#define ERROR_SIGNAL(early_error) Early_Error_Handler(early_error)

#ifdef __cplusplus
}
#endif

#endif //ERROR_H
