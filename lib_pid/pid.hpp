#ifndef PID_HPP
#define PID_HPP

#include <cstdint>

class PID{
public:
    PID()=default;
    PID(std::uint16_t Kp, std::uint16_t Ki, std::uint16_t Kd,
        std::uint16_t min_output, std::uint16_t max_output);

    std::uint16_t ComputeSpeed(std::uint16_t setpoint,
                               std::uint16_t input,
                               std::uint16_t dt_ms);
    ~PID()=default;

    void Reset();
    void SetGains(std::uint16_t Kp, std::uint16_t Ki, std::uint16_t Kd);

    std::uint16_t GetKp()const;
    std::uint16_t GetKi()const;
    std::uint16_t GetKd()const;

    std::int32_t GetIntegral()const;
    std::int32_t GetPrevInput()const;
    std::int32_t GetDerivFiltered()const;
    std::int32_t GetLastOutput()const;

public:
    struct PID_monitor{
        std::int32_t p;
        std::int32_t i;
        std::int32_t d;
        std::int32_t out;
    };
    const PID_monitor& GetMonitorData()const;

private:
    static constexpr std::uint16_t SCALE = 100;
    static constexpr std::uint16_t MAX_I_OUT = 80; //constraint for integral component Iout
    std::uint16_t Kp_{80};
    std::uint16_t Ki_{5};
    std::uint16_t Kd_{20};

    std::uint8_t min_output_{0};
    std::uint8_t max_output_{100};

    std::int32_t integral_{0};
    std::int32_t prev_input_{0};
    std::int32_t deriv_filtered_{0};
    std::int32_t last_output_{0};

    PID_monitor pid_monitor_{0};
};

#endif //PID_HPP
