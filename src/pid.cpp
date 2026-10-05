#include "pid.hpp"

static inline std::int32_t ClampInt32(std::int32_t val, std::int32_t lo, std::int32_t hi){
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

static inline std::uint16_t ClampUint16(std::int32_t val, std::uint16_t lo, std::uint16_t hi){
    if (val < lo) return lo;
    if (val > hi) return hi;
    return static_cast<std::uint16_t>(val);
}

PID::PID(std::uint16_t Kp, std::uint16_t Ki, std::uint16_t Kd, std::uint16_t min_output, std::uint16_t max_output)
    : Kp_(Kp)
    , Ki_(Ki)
    , Kd_(Kd)
    , min_output_(min_output)
    , max_output_(max_output){
        Reset();
    }

std::uint32_t PID::ComputeSpeed(std::uint16_t setpoint, std::uint16_t input, std::uint16_t dt_ms){

    auto dt_sec_scaled = static_cast<std::int32_t>(dt_ms)*SCALE/1000;
    if (dt_sec_scaled == 0){
        dt_sec_scaled=1;
    }

    auto error = static_cast<std::int32_t>(setpoint) - static_cast<std::int32_t>(input);

    // P: Kp * error
    auto Pout =static_cast<std::int32_t>(Kp_) * error / SCALE;

    // I: Ki * integral
    std::int32_t integral_inc = error * dt_sec_scaled / SCALE;
    std::int32_t integral_new = integral_ + integral_inc;
    std::int32_t max_integral = 0;

    if(Ki_!=0){
        max_integral = MAX_I_OUT * SCALE / static_cast<int32_t>(Ki_);
        integral_new = ClampInt32(integral_new, -max_integral, max_integral);
    }else{
        integral_new=0;
    }
    auto Iout = static_cast<std::int32_t>(Ki_) * integral_new / SCALE;

    // D:
    int32_t d_input = static_cast<int32_t>(input) - prev_input_;
    int32_t derivative = -d_input * SCALE / dt_sec_scaled;

    deriv_filtered_ = (7 * deriv_filtered_ + 3 * derivative) / 10;
    auto Dout =static_cast<std::int32_t>(Kd_) * deriv_filtered_ / SCALE;

    // P + I + D
    auto output = Pout + Iout + Dout;

    //Anti-windup
    if (output > max_output_){
        output = max_output_;
        if (error > 0) {
            integral_new = integral_;
        }
    }else if (output < min_output_) {
        output = min_output_;
        if (error < 0) {
            integral_new = integral_;
        }
    }
    integral_ = integral_new;

    prev_input_ = static_cast<int32_t>(input);
    last_output_ = output;

    return ClampUint16(output, min_output_, max_output_);
}

void PID::Reset(){
    integral_ = 0;
    prev_input_ = 0;
    deriv_filtered_ = 0;
    last_output_ = 0;
}

void PID::SetGains(std::uint16_t Kp, std::uint16_t Ki, std::uint16_t Kd){
    Kp_ = Kp;
    Ki_ = Ki;
    Kd_ = Kd;
    Reset();
}