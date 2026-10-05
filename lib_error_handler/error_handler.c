#include "error_handler.h"
#include "logger.h"
#include "stm32f1xx.h"

static ErrorCode_t last_error = error_none;
static const char* error_file = NULL;
static uint32_t error_line = 0;

void Error_Handler(){

    __disable_irq();

    while(1){
        GPIOB->ODR |= GPIO_ODR_ODR2;
    }
}

void Error_Handler_withCode(ErrorCode_t error_code, const char* file, uint32_t line){

    last_error=error_code;
    error_file=file;
    error_line=line;

    static volatile uint8_t in_error = 0; //trying use logger
    if(!in_error){
        in_error=1;

        switch (last_error) {
            case error_none: break;
            case uart_init_fail:
                LOG_ERROR("FATAL: UART initialization failed at %s:%lu", file, line);
                break;
            case spi_init_fail:
                LOG_ERROR("FATAL: SPI initialization failed at %s:%lu", file, line);
                break;
            case spi_transmit_fail:
                LOG_ERROR("FATAL: SPI transmitting failed at %s:%lu", file, line);
                break;
            case display_init_fail:
                LOG_ERROR("FATAL: Display initialization failed at %s:%lu", file, line);
                break;
            case display_comm_fail:
                LOG_ERROR("FATAL: Display commutation failed at %s:%lu", file, line);
                break;
            case assert_fail:
                LOG_ERROR("FATAL: assert failed at %s:%lu", file, line);
                break;
            case iwdg_fail:
                LOG_ERROR("FATAL: Whatchdog timeout at %s:%lu", file, line);
                break;
            case eeprom_fail:
                LOG_ERROR("FATAL: EEPROM failed at %s:%lu", file, line);
                break;
            case pointer_init_fail:
                LOG_ERROR("FATAL: Pointer Init failed at %s:%lu", file, line);
                break;
            default:
                LOG_ERROR("FATAL: Unknown error %d at %s:%lu", last_error, file, line);
                break;
        }
        in_error=0;
    }

    Error_Handler();
}

void assert_failed(uint8_t *file, uint32_t line){
    Error_Handler_withCode(assert_fail, (const char*)file, line);
}

// ===== error handler to signal issue by LED(PB2) untill UART is initialized =====

void Error_Led_Init(){

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    for(volatile int i = 0; i < 100; i++);

    GPIOB->CRL &= ~(0xF << 8);
    GPIOB->CRL |= (0x3 << 8);

    // LED OFF
    GPIOB->ODR &= ~(1 << 2);
}

void Error_Blinking(){
    GPIOB->ODR &= ~GPIO_ODR_ODR2; // Toggle LED
    for(volatile int i = 0; i < 1000000; i++);
    GPIOB->ODR |= GPIO_ODR_ODR2;
    for(volatile int i = 0; i < 500000; i++);
}

void Early_Error_Handler(EarlyErrorCode_t early_error){
    static size_t pause = 2000000;
    uint8_t n = (uint8_t)early_error;
    while(1) {
        for(uint8_t i = 0; i < n; i++) {
            Error_Blinking();
        }
        for(volatile size_t i = 0; i < pause; i++);  // Longer pause
    }
}
