#ifndef PROJECT_CONFIG_HPP
#define PROJECT_CONFIG_HPP

#include <cstdint>
#include <stm32f1xx_hal.h>
#include "tim.h"
#include "gpio.h"

struct ProjectConfig{
    std::uint16_t current_turns = 0;
    std::uint16_t total_turns = 1000;
    std::uint16_t feeder_counts = 0;
    std::uint16_t max_feeder_counts = 4800;
    std::uint8_t wire_diameter_scaled = 30;
    std::uint8_t Kp_scaled = 80;
    std::uint8_t Ki_scaled = 5;
    std::uint8_t Kd_scaled = 20;
    std::uint8_t can_read = 0;
    std::uint8_t need_write = 1;
};

struct UartCommand{
    std::uint16_t cmd;
    std::uint8_t index;
};

struct MotorConfig {
    static constexpr std::uint8_t MAX_SPEED = 250;
    static constexpr std::uint8_t MIN_SPEED = 70;
};

/*for motor driver BTS7960*/
struct BTS7960ConfigMotor : public MotorConfig {
    // Using TIM4
    static constexpr TIM_HandleTypeDef* TIMER = &htim4;
    static constexpr std::uint32_t TIMER_CHANNEL_FWD = TIM_CHANNEL_1;
    static constexpr std::uint32_t TIMER_CHANNEL_REV = TIM_CHANNEL_2;
    static constexpr bool IS_COMPLEMENTARY = false;
    static constexpr std::uint16_t TIMER_PERIOD = 2047;
    // PWM pins
    static constexpr std::uint16_t R_PWM_PIN = RPWM_MOTOR_PIN;
    static constexpr std::uint16_t L_PWM_PIN = LPWM_MOTOR_PIN;
    // Enable pins
    inline static GPIO_TypeDef* ENABLE_PORT_L = GPIOB;
    inline static GPIO_TypeDef* ENABLE_PORT_R = GPIOB;
    static constexpr std::uint16_t L_PIN = L_EN_MOTOR_PIN;
    static constexpr std::uint16_t R_PIN = R_EN_MOTOR_PIN;
};

/*for feeder driver BTS7960*/
struct BTS7960ConfigFeeder : public MotorConfig {
    // Using TIM1
    static constexpr TIM_HandleTypeDef* TIMER = &htim1;
    static constexpr std::uint32_t TIMER_CHANNEL_FWD = TIM_CHANNEL_1;
    static constexpr std::uint32_t TIMER_CHANNEL_REV = TIM_CHANNEL_4;
    static constexpr bool IS_COMPLEMENTARY = false;
    static constexpr std::uint16_t TIMER_PERIOD = 4095;
    // PWM pins
    static constexpr std::uint16_t R_PWM_PIN = RPWM_FEEDER_PIN;
    static constexpr std::uint16_t L_PWM_PIN = LPWM_FEEDER_PIN;
    // Enable pins
    inline static GPIO_TypeDef* ENABLE_PORT_L = GPIOB;
    inline static GPIO_TypeDef* ENABLE_PORT_R = GPIOB;
    static constexpr std::uint16_t L_PIN = L_EN_FEEDER_PIN;
    static constexpr std::uint16_t R_PIN = R_EN_FEEDER_PIN;
};

#if 0
/*for feeder driver DRV8871*/
struct DRV8871Config : public MotorConfig{
    //Using TIM1
    static constexpr TIM_HandleTypeDef* TIMER = &htim1;
    static constexpr std::uint32_t TIMER_CHANNEL_FWD = TIM_CHANNEL_1;
    static constexpr std::uint32_t TIMER_CHANNEL_REV = TIM_CHANNEL_2;
    static constexpr std::uint16_t TIMER_PERIOD = 4095;
    static constexpr bool IS_COMPLEMENTARY = true;
    //PWM pins
    inline static GPIO_TypeDef* PWM_PORT = GPIOB;
    static constexpr std::uint16_t R_PWM_PIN = IN1_PIN;
    static constexpr std::uint16_t L_PWM_PIN = IN2_PIN;
};
#endif

#endif //PROJECT_CONFIG_HPP