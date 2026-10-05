#include <cstdint>

#include <stm32f1xx_hal.h>
#include <logger.h>
#include <logger_with_hal.h>
#include <error_handler.h>

#include "eeprom_data_init.hpp"
#include "eeprom.h"

void EEPROM_Data_Init(const std::uint16_t VirtAddrVarTab[0], ProjectConfig &config){

    HAL_FLASH_Unlock();

    if(EE_Init()!=HAL_OK){
        LOG_ERROR("EE_Init() is failed");
        ERROR_REPORT(eeprom_fail);
        Error_Handler();
    }

    std::uint16_t data[NB_OF_VAR]={0};

    // Read all variables from EEPROM
    for(std::uint8_t i=0; i<NB_OF_VAR; ++i){
        if(EE_ReadVariable(VirtAddrVarTab[i], &data[i])!=HAL_OK){
            LOG_ERROR("EEPROM variable %d read failed", i);
            break;
        }
    }

    if(data[VAR_CAN_READ]){
        config.current_turns=data[0];
        config.total_turns=data[1];
        config.feeder_counts=data[2];
        config.max_feeder_counts=data[3];
        config.wire_diameter_scaled=data[4];
        config.Kp_scaled=data[5];
        config.Ki_scaled=data[6];
        config.Kd_scaled=data[7];
        config.can_read = 1;
        config.need_write = 0;
        LOG_INFO("CONFIG loaded from EEPROM");
        return;
    }

    // New winding parameters by default
    std::uint32_t init_param[NB_OF_VAR] ={ config.current_turns,
                                           config.total_turns,
                                           config.feeder_counts,
                                           config.max_feeder_counts,
                                           config.wire_diameter_scaled,
                                           config.Kp_scaled,
                                           config.Ki_scaled,
                                           config.Kd_scaled,
                                           config.can_read,
                                           config.need_write};

    for(std::uint8_t i=0; i<NB_OF_VAR; ++i){
        auto status = EE_WriteVariable(VirtAddrVarTab[i], init_param[i]);
        if (status != HAL_OK){
            LOG_ERROR("Failed to write to EEPROM parameter=%d, index=%d", init_param[i], i);
            Error_Handler();
        }
    }
    LOG_INFO("Default CONFIG written to EEPROM");
}