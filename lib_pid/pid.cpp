#include "pid.hpp"

static inline std::int32_t ClampInt32(std::int32_t val, std::int32_t lo, std::int32_t hi){
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

PID::PID(std::uint16_t Kp, std::uint16_t Ki, std::uint16_t Kd, std::uint16_t min_output, std::uint16_t max_output)
    : Kp_(Kp)
    , Ki_(Ki)
    , Kd_(Kd)
    , min_output_(min_output)
    , max_output_(max_output){
        Reset();
    }

std::uint16_t PID::ComputeSpeed(std::uint16_t setpoint, std::uint16_t input, std::uint16_t dt_ms){

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
        max_integral = MAX_I_OUT * SCALE / static_cast<std::int32_t>(Ki_);
        integral_new = ClampInt32(integral_new, -max_integral, max_integral);
    }else{
        integral_new=0;
    }
    auto Iout = static_cast<std::int32_t>(Ki_) * integral_new / SCALE;

    // D:
    std::int32_t d_input = static_cast<std::int32_t>(input) - prev_input_;
    std::int32_t derivative = -d_input * SCALE / dt_sec_scaled;

    deriv_filtered_ = (7 * deriv_filtered_ + 3 * derivative) / 10;
    auto Dout =static_cast<std::int32_t>(Kd_) * deriv_filtered_ / SCALE;

    // P + I + D
    std::int32_t output = Pout + Iout + Dout;

    //for unit-tests
    pid_monitor_.p=Pout;
    pid_monitor_.i=Iout;
    pid_monitor_.d=Dout;
    pid_monitor_.out=output;

    //Anti-windup
    bool saturated = false;
    if (output > max_output_){
        output = max_output_;
        saturated=true;
    }else if (output < min_output_) {
        output = min_output_;
        saturated=true;
    }

    if(!saturated){
        integral_ = integral_new;
    }

    prev_input_ = static_cast<std::int32_t>(input);
    last_output_ = output;

    return static_cast<std::uint16_t>(output);
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

std::uint16_t PID::GetKp()const{
    return Kp_;
}

std::uint16_t PID::GetKi()const{
    return Ki_;
}

std::uint16_t PID::GetKd()const{
    return Kd_;
}

std::int32_t PID::GetIntegral()const{
    return integral_;
}

std::int32_t PID::GetPrevInput()const{
    return prev_input_;
}

std::int32_t PID::GetDerivFiltered()const{
    return deriv_filtered_;
}

std::int32_t PID::GetLastOutput()const{
    return last_output_;
}

const PID::PID_monitor& PID::GetMonitorData()const{
    return pid_monitor_;
}