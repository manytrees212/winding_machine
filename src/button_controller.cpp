#include "button_controller.hpp"

#include <cstdint>
#include <stm32f1xx_hal.h>

#include <logger.h>
#include <logger_with_hal.h>
#include <gpio.h>

void ButtonController::ScanButtons(){

    auto current_time = HAL_GetTick();
    if(current_time < next_update_time_){
        return;
    }
    next_update_time_=current_time+SCAN_UPDATE_INTERVAL_MS;

    while (current_time > next_update_time_) {
        next_update_time_ += SCAN_UPDATE_INTERVAL_MS;
    }

    const uint16_t rows[ROWS]={ROW1_PIN, ROW2_PIN};
    const uint16_t cols[COLS] = {COL1_PIN, COL2_PIN, COL3_PIN};

    HAL_GPIO_WritePin(GPIOA, ROW1_PIN | ROW2_PIN, GPIO_PIN_SET);
    for (uint8_t i = 0; i < 10; i++);

    for(uint16_t row=0; row < ROWS; ++row){

        HAL_GPIO_WritePin(GPIOA, rows[row], GPIO_PIN_RESET);
        for (uint8_t i = 0; i < 10; i++);

        for(uint16_t col=0; col < COLS; ++col){

            GPIO_PinState pin_state = HAL_GPIO_ReadPin(GPIOB, cols[col]);
            bool current_state = (pin_state == GPIO_PIN_RESET);            
            if(current_state) {                
                if (pushed_counter_[row][col] < DEBOUNCE_THRESHOLD) {
                    pushed_counter_[row][col]++;
                }
            }else{
                pushed_counter_[row][col] = 0;
                last_stable_state_[row][col] = current_state;
            }

            if(pushed_counter_[row][col] >= DEBOUNCE_THRESHOLD){
                bool low_stable_state = current_state;
                // Detect edge transitions (RESET && !SET)
                if(low_stable_state && !last_stable_state_[row][col]){
                    SetPushedButton(button_matrix_[row][col]);
                }
                last_stable_state_[row][col] = low_stable_state;
            }
        }
        HAL_GPIO_WritePin(GPIOA, rows[row], GPIO_PIN_SET);
        for (uint8_t i = 0; i < 10; i++);
    }
}

uint8_t ButtonController::GetPushedButton(){

    __disable_irq();

    uint8_t button = pushed_button_;
    pushed_button_ = 0;

    __enable_irq();

    return button;
}

void ButtonController::SetPushedButton(const uint8_t button){
    if(pushed_button_ == 0){
        pushed_button_ = button;
    }
}

void ButtonController::ResetPushedButton(){
    pushed_button_=0;
}