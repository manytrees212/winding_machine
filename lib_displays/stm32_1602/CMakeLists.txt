cmake_minimum_required(VERSION 3.20)

add_library(stm32_1602 STATIC)

target_sources(stm32_1602 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lcd_1602.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lcd_1602.h
)

target_include_directories(stm32_1602 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)


message(STATUS "stm32_1602 library is configured")
