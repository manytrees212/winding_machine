#include "debounce.hpp"

void Debounce::Update(uint32_t current_tick) {
    if (gpio_port_ == nullptr) return;

    system_tick_ = current_tick;
    bool raw_state = (gpio_port_->IDR & pin_mask_) != 0; // true если HIGH

    switch (state_) {
        case State::StableHigh:
            if (!raw_state) {
                state_ = State::BouncingDown;
                last_change_tick_ = system_tick_;
            }
            break;

        case State::BouncingDown:
            if (raw_state) {
                state_ = State::StableHigh; // Возврат к высокому уровню
            } else if ((system_tick_ - last_change_tick_) >= filter_ticks_) {
                state_ = State::StableLow;
                released_flag_ = false; // Сброс флага отпускания при нажатии
            }
            break;

        case State::StableLow:
            if (raw_state) {
                state_ = State::BouncingUp;
                last_change_tick_ = system_tick_;
            }
            break;

        case State::BouncingUp:
            if (!raw_state) {
                state_ = State::StableLow; // Возврат к низкому уровню
            } else if ((system_tick_ - last_change_tick_) >= filter_ticks_) {
                state_ = State::StableHigh;
                released_flag_ = true; // Фиксируем факт отпускания кнопки
            }
            break;
    }
}

bool Debounce::IsPressed() const {
    return state_ == State::StableLow;
}

bool Debounce::WasReleased() const {
    return released_flag_;
}