#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <cstdint>
#include <stm32f1xx_hal.h>
#include <type_traits>

#include "project_config.hpp"

enum class MotorState {Idle, Running};
enum class Direction  {Forward, Reverse};

template <typename T, typename = void>
struct has_enable_pins : std::false_type {};

template <typename T>
struct has_enable_pins<T, std::void_t<decltype(T::ENABLE_PORT)>> : std::true_type {};

template <typename Config>
class MotorDriver{
public:
    MotorDriver(){
        if constexpr (Config::IS_COMPLEMENTARY) {
            HAL_TIMEx_PWMN_Start(Config::TIMER, Config::TIMER_CHANNEL_FWD);
            HAL_TIMEx_PWMN_Start(Config::TIMER, Config::TIMER_CHANNEL_REV);
        } else {
            HAL_TIM_PWM_Start(Config::TIMER, Config::TIMER_CHANNEL_FWD);
            HAL_TIM_PWM_Start(Config::TIMER, Config::TIMER_CHANNEL_REV);
        }
        ApplyPWM(0);
    }
    ~MotorDriver()=default;

    void Start(){
        if(state_==MotorState::Running) {return;}
        ApplyPWM(speed_);
        EnableMotor(true);
        state_ = MotorState::Running;
    }

    void Stop(){
        if(state_==MotorState::Idle) {return;}
        ApplyPWM(0);
        EnableMotor(false);
        state_ = MotorState::Idle;
    }

    void SetDirection(Direction dir){
        if(state_ == MotorState::Running) {Stop();}
        direction_=dir;
    }

    void SetSpeed(uint8_t speed){
        if(speed>Config::MAX_SPEED){
            speed=Config::MAX_SPEED;
        }
        speed_=speed;
        if (state_ == MotorState::Running){
            ApplyPWM(speed);
        }
    }

    auto GetState()const{return state_;}
    auto GetSpeed()const{return speed_;}
    auto GetMaxSpeed()const{return Config::MAX_SPEED;}
    auto GetMinSpeed()const{return Config::MIN_SPEED;}
    auto GetDirection()const{return direction_;}

private:
    static constexpr bool HasEnablePins(){
        return has_enable_pins<Config>::value;
    }

    void EnableMotor(bool enable) {
        if constexpr (HasEnablePins()) {
            GPIO_PinState state = enable ? GPIO_PIN_SET : GPIO_PIN_RESET;
            HAL_GPIO_WritePin(Config::ENABLE_PORT, Config::L_PIN, state);
            HAL_GPIO_WritePin(Config::ENABLE_PORT, Config::R_PIN, state);
        }
    }

    void ApplyPWM(uint8_t speed){

        static_assert(Config::MAX_SPEED > 0, "MAX_SPEED must be greater than 0");

        std::uint32_t compare=0;
        if (speed > 0) {
            if constexpr (Config::TIMER_PERIOD == 4095) {
                compare = static_cast<uint32_t>(speed) << 4; // Fast multiply by 16
            } else if constexpr (Config::TIMER_PERIOD == 2047) {
                compare = static_cast<uint32_t>(speed) << 3; // Fast multiply by 8
            } else {
                compare = (static_cast<uint32_t>(speed) * Config::TIMER->Init.Period) / MotorConfig::MAX_SPEED;
            }
        }

        if(compare>0){
            switch(direction_){
            case Direction::Forward:
                __HAL_TIM_SET_COMPARE(Config::TIMER, Config::TIMER_CHANNEL_FWD, compare);
                __HAL_TIM_SET_COMPARE(Config::TIMER, Config::TIMER_CHANNEL_REV, 0);
                break;
            case Direction::Reverse:
                __HAL_TIM_SET_COMPARE(Config::TIMER, Config::TIMER_CHANNEL_FWD, 0);
                __HAL_TIM_SET_COMPARE(Config::TIMER, Config::TIMER_CHANNEL_REV, compare);
                break;
            default: break;
            }
        }else{
            __HAL_TIM_SET_COMPARE(Config::TIMER, Config::TIMER_CHANNEL_FWD, 0);
            __HAL_TIM_SET_COMPARE(Config::TIMER, Config::TIMER_CHANNEL_REV, 0);
        }
    }

private:
    MotorState state_ = MotorState::Idle;
    Direction direction_ = Direction::Forward;
    uint8_t speed_ = 0;
};

#endif //MOTOR_HPP