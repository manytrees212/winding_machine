#include "flash_data_controller.hpp"
#include "project_config.hpp"
#include <eeprom.h>
#include <logger.h>
#include <logger_with_hal.h>
#include <error_handler.h>

FlashDataController::FlashDataController(ProjectConfig* config,
                                         const std::uint16_t *VirtAddrVarTab)
    :config_(config)
    , virt_addr_var_tab_(VirtAddrVarTab){
        if(virt_addr_var_tab_ == nullptr){
            LOG_ERROR("Virtual address table pointer is null");
            Error_Handler();
        }
    }

void FlashDataController::WriteFlashData(){
    std::uint16_t write_data[NB_OF_VAR] = {
        config_->current_turns,
        config_->total_turns,        
        config_->feeder_counts,
        config_->max_feeder_counts,
        config_->wire_diameter_scaled,
        config_->Kp_scaled,
        config_->Ki_scaled,
        config_->Kd_scaled,
        1,  // can_read = valid
        0   // need_write
    };
    for(std::uint8_t i = 0; i < NB_OF_VAR; ++i){
        if(EE_WriteVariable(virt_addr_var_tab_[i], write_data[i])!=HAL_OK){
            LOG_ERROR("Flash was not written");
            return;
        }
    }
    LOG_INFO("FlashData written:\
            current_turns=%d,\
            total_turns=%d,\
            feeder_counts=%d,\
            max_feeder_counts=%d,\
            wire_diameter=%d,\
            Kp_scaled=%d,\
            Ki_scaled=%d,\
            Kd_scaled=%d",
            config_->current_turns,
            config_->total_turns,
            config_->feeder_counts,
            config_->max_feeder_counts,
            config_->wire_diameter_scaled,
            config_->Kp_scaled,
            config_->Ki_scaled,
            config_->Kd_scaled);
}

void FlashDataController::ResetFlashData(){
    std::uint16_t reset_data[NB_OF_VAR]={0,0,0,0,0,0,0,0,0,1};
    for(std::uint8_t i = 0; i < NB_OF_VAR; ++i){
        if(EE_WriteVariable(virt_addr_var_tab_[i], reset_data[i])!=HAL_OK){
            LOG_ERROR("Flash was not reseted");
            return;
        }
    }
    LOG_INFO("FLASH has been RESET");
}

void FlashDataController::ProcessUartCommand(const UartCommand cmd){
    auto cmd_index = cmd.index;
    auto value = cmd.cmd;
    if(cmd_index == 0 || cmd_index > 8){
        LOG_INFO("UartCommand: index=%d , cmd=%d NOT APPLIED", cmd_index, value);
        return;
    }

    switch(cmd_index){
        case 1: /*if(value <= config_->total_turns) config_->current_turns = value;*/ break;
        case 2: if(value <= 2000)   config_->total_turns = value; break;
        case 3: /*if(value <= 6000) config_->feeder_counts = value;*/ break;
        case 4: if(value <= 4800)   config_->max_feeder_counts = value; break;
        case 5: if (value <= 200)   config_->wire_diameter_scaled = value; break;
        case 6: if (value <= 100)   {config_->Kp_scaled = value; SetPidCoefUpdFlag();} break;
        case 7: if (value <= 80)    {config_->Ki_scaled = value; SetPidCoefUpdFlag();} break;
        case 8: if (value <= 100)   {config_->Kd_scaled = value; SetPidCoefUpdFlag();} break;
        default:
            LOG_INFO("Unknown command index=%d", cmd_index);
    }
}

void FlashDataController::SetFlashCurrentTurns(const std::uint16_t turns_number){
    if(turns_number<=config_->total_turns){
        config_->current_turns=turns_number;
    }
}

void FlashDataController::SetFlashFeederCounts(const std::uint16_t feeder_counts){
    config_->feeder_counts=feeder_counts;
}

void FlashDataController::SetMaxFlashFeederCounts(const std::uint16_t feeder_counts){
    config_->max_feeder_counts = feeder_counts;
}

void FlashDataController::SetKpScaled(const std::uint8_t Kp){
    if(Kp<100) config_->Kp_scaled = Kp;
}

void FlashDataController::SetKiScaled(const std::uint8_t Ki){
    if(Ki<100) config_->Ki_scaled = Ki;
}

void FlashDataController::SetKdScaled(const std::uint8_t Kd){
    if(Kd<100) config_->Kd_scaled = Kd;
}

std::uint8_t FlashDataController::GetScaleOfWireDiam()const{
    return SCALE;
}

std::uint16_t FlashDataController::GetScaledFlashWireDiam()const{
    return config_->wire_diameter_scaled;
}

std::uint16_t FlashDataController::GetFlashTotalTurns() const{
    return config_->total_turns;
}

std::uint16_t FlashDataController::GetFlashCurrentTurns() const{
    return config_->current_turns;
}

std::uint16_t FlashDataController::GetFlashFeederCounts() const{
    return config_->feeder_counts;
}

std::uint16_t FlashDataController::GetMaxFlashFeederCounts()const{
    return config_->max_feeder_counts;
}

std::uint16_t FlashDataController::GetMaxFlashFeederPosScaled(){
    return (config_->max_feeder_counts*SCALE/32);
}

std::uint8_t  FlashDataController::GetKpScaled()const{
    return config_->Kp_scaled;
}

std::uint8_t  FlashDataController::GetKiScaled()const{
    return config_->Ki_scaled;
}

std::uint8_t  FlashDataController::GetKdScaled()const{
    return config_->Kd_scaled;
}

bool FlashDataController::GetPidCoefUpdFlag()const{
    return PID_coef_updated_flag;
}

void FlashDataController::SetPidCoefUpdFlag(){
    PID_coef_updated_flag=true;
}

void FlashDataController::ResetPidCoefUpdFlag(){
    PID_coef_updated_flag=false;
}