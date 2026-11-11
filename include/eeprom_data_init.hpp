#ifndef EEPROM_DATA_INIT_HPP
#define EEPROM_DATA_INIT_HPP

#include <eeprom.h>

#include <cstdint>
#include "project_config.hpp"

#define VAR_CURRENT_TURNS_1         0
#define VAR_CURRENT_TURNS_2         1
#define VAR_TOTAL_TURNS             2
#define VAR_FEEDER_COUNTS           3
#define VAR_MAX_FEEDER_COUNTS       4
#define VAR_WIRE_DIAMETER_SCALED    5
#define VAR_KP_SCALED               6
#define VAR_KI_SCALED               7
#define VAR_KD_SCALED               8
#define VAR_CAN_READ                9
#define VAR_NEED_WRITE              10

void EEPROM_Data_Init(const std::uint16_t (&VirtAddrVarTab)[NB_OF_VAR], ProjectConfig &config);

#endif