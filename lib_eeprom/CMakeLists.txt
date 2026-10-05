cmake_minimum_required(VERSION 3.20)

add_library(eeprom STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/eeprom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/eeprom.h
)

target_include_directories(eeprom PUBLIC
     ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(eeprom PUBLIC
    HAL_Driver
)

message(STATUS "eeprom library is configured")
