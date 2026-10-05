cmake_minimum_required(VERSION 3.20)

add_library(stm32_st7789 STATIC)

target_sources(stm32_st7789 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/st7789.c
    ${CMAKE_CURRENT_SOURCE_DIR}/st7789.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts.c
)

target_include_directories(stm32_st7789 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#dependencies
target_link_libraries(stm32_st7789 PUBLIC
    HAL_Driver
    logger
)

message(STATUS "stm32_st7789 library is configured")
