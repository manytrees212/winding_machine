cmake_minimum_required(VERSION 3.20)

add_library(logger STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger_with_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logger_with_hal.h
)

target_include_directories(logger PUBLIC
     ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logger PUBLIC
    HAL_Driver
)

message(STATUS "logger library is configured")
