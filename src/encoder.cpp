#include "encoder.hpp"
#include "project_config.hpp"
#include "logger.h"
#include "logger_with_hal.h"

Encoder::Encoder(TIM_HandleTypeDef* htim)
    : htim_(htim){
    if(htim_){
        __HAL_TIM_SET_COUNTER(htim_, CNT_MIDDLE);
    }
    LOG_INFO("Ctor ENCODER: time=%d", GetCounts());
}

std::uint16_t Encoder::GetCounts()const{
    if(htim_){
        return __HAL_TIM_GET_COUNTER(htim_);
    }
    return ERROR_CNT;
}

void Encoder::SetCounter(std::uint16_t counts){
    if(!htim_){
      LOG_ERROR("Ctor ENCODER: timer invalid");
      return;
    }
    __HAL_TIM_SET_COUNTER(htim_, counts);
}

void Encoder::DisableCounting(){
    if(!htim_){
        LOG_ERROR("Ctor ENCODER: timer invalid");
        return;
    }
    HAL_TIM_Encoder_Stop(htim_, TIM_CHANNEL_ALL);
}

void Encoder::EnableCounting(){
    if(!htim_){
        LOG_ERROR("Ctor ENCODER: timer invalid");
        return;
    }
    HAL_TIM_Encoder_Start(htim_, TIM_CHANNEL_ALL);
}

EncoderController::EncoderController(TIM_HandleTypeDef* htim_ptr1,
                                     TIM_HandleTypeDef* htim_ptr2,
                                     std::uint16_t init_turns)
    :motor_encoder_(htim_ptr1)
    ,feeder_encoder_(htim_ptr2)
    ,turn_number_scaled_(init_turns*SCALE_FACTOR_TURNS){
        for(std::uint8_t i=0; i<BUFF_SIZE; ++i){
            bobbin_rpm_buff_[i]=0;
        }
        LOG_INFO("Ctor ENCODER_CTRL: motor_tim_cnt=%d", motor_encoder_.GetCounts());
        LOG_INFO("Ctor ENCODER_CTRL: feeder_tim_cnt=%d", feeder_encoder_.GetCounts());
    }

std::uint16_t EncoderController::GetTurnNumber()const{
    auto real_turn = static_cast<std::int16_t>(turn_number_scaled_/SCALE_FACTOR_TURNS);
    return real_turn;
}

std::uint32_t EncoderController::GetTurnNumberScaled()const{
    return turn_number_scaled_;
}

std::uint32_t EncoderController::GetLastTurnNumberScaled()const{
    return last_turn_number_scaled_;
}

std::uint16_t EncoderController::GetBobbinRPM()const{
    return bobbin_rpm_scaled_/SCALE_FACTOR_RMP;
}

std::uint16_t EncoderController::GetBobbinRPM_Scaled()const{
    return bobbin_rpm_scaled_;
}

std::int32_t EncoderController::GetFeederCounts(){
    auto current_feeder_counts = feeder_encoder_.GetCounts();
    auto delta = static_cast<std::int16_t>(current_feeder_counts - last_feeder_counts_);
    extended_feeder_counts_+=delta;
    last_feeder_counts_=current_feeder_counts;
    return extended_feeder_counts_;
}

std::uint8_t EncoderController::GetCountsPerRevolution()const{
    return REAL_COUNTS_PER_REV;
}

std::uint8_t EncoderController::GetScaleFactorTurns()const{
    return SCALE_FACTOR_TURNS;
}

void EncoderController::SetFeederCounts(const std::int32_t counts){
    feeder_encoder_.SetCounter(counts);
}

void EncoderController::ResetFeederCounts(){
    feeder_encoder_.SetCounter(0);
    extended_feeder_counts_=0;
    last_feeder_counts_=0;
}

std::uint16_t EncoderController::FindMajorityValue(std::uint16_t* buff, std::uint16_t size){

    if(!buff || size == 0) return 0;

    std::uint16_t most_freq = buff[0];
    std::uint16_t cnt = 1;

    /*not full algorithm of searching the majority*/
    for(std::uint8_t i=1; i<size; ++i){
        if(buff[i]==most_freq && most_freq!=0){
            ++cnt;
        }else{
            --cnt;
        }
        if(cnt==0){
            most_freq=buff[i];
            ++cnt;
        }
    }
    return most_freq;
}

void EncoderController::UpdateTurnNumber(){
    std::uint16_t motor_current_counts = motor_encoder_.GetCounts();
    motor_encoder_.SetCounter(CNT_MIDDLE);

    motor_counts_per_update_ = motor_current_counts - CNT_MIDDLE;
    if(motor_counts_per_update_==0){
        motor_encoder_has_updated_=false;
        return;
    }

    auto motor_turns_number_scaled = static_cast<std::int32_t>((motor_counts_per_update_ * SCALE_FACTOR_TURNS) / REAL_COUNTS_PER_REV); // motor output
    last_turn_number_scaled_=turn_number_scaled_;
    turn_number_scaled_+=(motor_turns_number_scaled*GEAR_RATIO_SCALED/SCALE_FACTOR_RATIO); //bobbin turns
    if(turn_number_scaled_<0){
        turn_number_scaled_=0;
    }

    motor_encoder_has_updated_=true;
}

void EncoderController::ReadMotorEncoderData(){

    UpdateTurnNumber();

    if(!motor_encoder_has_updated_){
        return;
    }

    // calc bobbin RMP:
    static std::uint16_t index = 0;
    if(index==BUFF_SIZE) index = 0;

    std::int32_t rpm_scaled_raw = (motor_counts_per_update_ * UPDATE_INTERVAL_MIN * SCALE_FACTOR_RMP) / REAL_COUNTS_PER_REV;
    if (rpm_scaled_raw < 0){
        rpm_scaled_raw = -rpm_scaled_raw;
    }

    auto rpm_scaled_=static_cast<std::uint16_t>(rpm_scaled_raw*GEAR_RATIO_SCALED/SCALE_FACTOR_RATIO);
    bobbin_rpm_buff_[index]=rpm_scaled_;

    bobbin_rpm_scaled_=FindMajorityValue(&bobbin_rpm_buff_[0], BUFF_SIZE);

    ++index;
}

void EncoderController::UpdateMotorEncoderData(){

    std::uint32_t current_time = HAL_GetTick();
    if ((current_time - last_motor_update_time_) < UPDATE_INTERVAL_MS){
        motor_encoder_has_updated_=false;
        return;
    }
    last_motor_update_time_ = current_time;

    ReadMotorEncoderData();
}

void EncoderController::UpdateFeederEncoderData(){

    std::uint32_t current_time = HAL_GetTick();
    if ((current_time - last_feeder_update_time_) < UPDATE_INTERVAL_MS){
        feeder_encoder_has_updated_=false;
        return;
    }
    last_feeder_update_time_ = current_time;

    auto feeder_counts = static_cast<std::uint16_t>(GetFeederCounts());
    if(feeder_counts!=last_update_feeder_counts_){
        feeder_encoder_has_updated_=true;
        last_update_feeder_counts_=feeder_counts;
    }else{
        feeder_encoder_has_updated_=false;
    }
}

void EncoderController::StopFeederCounting(){
    feeder_encoder_.DisableCounting();
    LOG_INFO("FEEDER ENCODER COUNTING STOPPED");
}

void EncoderController::StartFeederCounting(){
    feeder_encoder_.EnableCounting();
    LOG_INFO("FEEDER ENCODER COUNTING STARTED");
}

void EncoderController::StopMotorCounting(){
    motor_encoder_.DisableCounting();
}

void EncoderController::StartMotorCounting(){
    motor_encoder_.EnableCounting();
}

bool EncoderController::MotorEncoderHasUpdated()const{
    return motor_encoder_has_updated_;
}

bool EncoderController::FeederEncoderHasUpdated()const{
    return feeder_encoder_has_updated_;
}