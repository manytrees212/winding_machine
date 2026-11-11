#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <cstdint>
#include <stm32f1xx_hal.h>

static constexpr std::uint32_t  CNT_MIDDLE = 32767;
static constexpr std::uint32_t ERROR_CNT = 54079;

class Encoder final{
public:
    Encoder()=delete;
    explicit Encoder(TIM_HandleTypeDef* htim);
    ~Encoder()=default;

    Encoder(const Encoder&)=delete;
    Encoder& operator=(const Encoder&)=delete;
    Encoder(Encoder&&)=delete;
    Encoder& operator=(Encoder&&)=delete;

    std::uint32_t GetCounts()const;
    void SetCounter(std::uint32_t counts);
    void DisableCounting();
    void EnableCounting();
private:
    TIM_HandleTypeDef* const htim_;
};

class EncoderController final{
public:
    EncoderController()=delete;
    EncoderController(TIM_HandleTypeDef* tim_ptr1,
                      TIM_HandleTypeDef* tim_ptr2,
                      std::uint32_t init_turns_scaled);

    EncoderController(const EncoderController&)=delete;
    EncoderController& operator=(const EncoderController&)=delete;
    EncoderController(EncoderController&&)=delete;
    EncoderController& operator=(EncoderController&&)=delete;
    ~EncoderController()=default;

    void UpdateMotorEncoderData();
    void UpdateFeederEncoderData();

    void UpdateTurnNumber();
    void UpdateMaxFeederCounts();
    void ResetFeederCounts();
    void SetFeederCounts(const std::uint16_t counts);

    void StartFeederCounting();
    void StopFeederCounting();

    void StartMotorCounting();
    void StopMotorCounting();

    std::int32_t GetTurnNumber()const;
    std::uint32_t GetTurnNumberScaled()const;
    std::uint32_t GetLastTurnNumberScaled()const;

    std::uint16_t GetMotorCounts()const;
    std::int32_t GetFeederCounts();

    std::uint16_t GetBobbinRPM()const;
    std::uint16_t GetBobbinRPM_Scaled()const;

    std::uint8_t GetCountsPerRevolution()const;
    std::uint8_t GetScaleFactorTurns()const;

    bool MotorEncoderHasUpdated()const;
    bool FeederEncoderHasUpdated()const;

private:
    void ReadMotorEncoderData();
    void UpdateMaxFeederPos();
private:
    static constexpr std::uint8_t   HW_COUNTS_PER_REVOLUTION = 8;
    static constexpr std::uint8_t   REAL_COUNTS_PER_REV = HW_COUNTS_PER_REVOLUTION * 4; // TIM_ENCODERMODE_TI12;
    static constexpr std::uint8_t   UPDATE_INTERVAL_MS = 100;
    static constexpr std::uint16_t  UPDATE_INTERVAL_MIN = (1000/UPDATE_INTERVAL_MS)*60;
    static constexpr std::uint8_t   GEAR_RATIO_NUM = 19; // z1 of gear
    static constexpr std::uint8_t   GEAR_RATIO_DEN = 76; // z2 of slave gear
    static constexpr std::uint16_t  SCALE_FACTOR_RATIO = 1000;
    static constexpr std::uint16_t  GEAR_RATIO_SCALED = GEAR_RATIO_NUM * SCALE_FACTOR_RATIO / GEAR_RATIO_DEN;
    static constexpr std::uint8_t   SCALE_FACTOR_TURNS = 100;
    static constexpr std::uint8_t   SCALE_FACTOR_RMP = 10;
private:
    Encoder motor_encoder_;
    Encoder feeder_encoder_;
    std::int32_t  turn_number_scaled_;

    std::uint32_t last_motor_update_time_{0};
    std::uint32_t last_feeder_update_time_{0};
    std::int32_t extended_feeder_counts_{0};
    std::uint32_t last_feeder_counts_{0};
    std::uint32_t last_update_feeder_counts_{0};

    std::int32_t  last_turn_number_scaled_ = 0;
    std::int16_t  motor_counts_per_update_= 0;
    std::uint16_t bobbin_rpm_scaled_ = 0;

    bool motor_encoder_has_updated_{false};
    bool feeder_encoder_has_updated_{false};
};

#endif //ENCODER_HPP