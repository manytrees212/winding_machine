#include "uart_cmd_parser.hpp"
#include "uart_init.h"
#include <stm32f1xx_hal.h>

void UartCmdParser::ParseUartBuffer(){

    const std::uint32_t timeout = 100;

    while (UART1_Data_Available()){
        std::uint8_t ch = UART1_Read();
        auto now = HAL_GetTick();

        if (now - last_time_ > timeout && pos_ > 0){
            pos_ = 0;
        }
        last_time_ = now;

        if (ch >= '0' && ch <= '9'){
            if (pos_ < sizeof(uart_cmd_buffer_) - 2){
                uart_cmd_buffer_[pos_++] = ch;
            }
        }else if (ch == '\r' || ch == '\n' || ch == ' ' || ch == ','){
            if (pos_ > 0) {
                uart_cmd_buffer_[pos_] = '\0';
                UpdateCmd();
                uart_cmd_ready_ = true;
                pos_ = 0;
            }
        }else{
            pos_ = 0;
        }
    }
}

void UartCmdParser::UpdateCmd(){
    if (pos_ == 0) return;

    uart_cmd_t_.index = uart_cmd_buffer_[0] - '0';

    uart_cmd_t_.cmd = 0;
    for (std::uint8_t i = 1; i < pos_; ++i) {
        uart_cmd_t_.cmd = uart_cmd_t_.cmd * 10 + (uart_cmd_buffer_[i] - '0');
    }
}

const UartCommand UartCmdParser::GetUartCmd(){
    if (uart_cmd_ready_) {
        uart_cmd_ready_ = false;
        return uart_cmd_t_;
    }
    return {0, 0};
}

bool UartCmdParser::IsCmdReady()const{
    return uart_cmd_ready_;
}