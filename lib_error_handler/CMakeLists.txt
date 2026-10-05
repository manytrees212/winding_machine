cmake_minimum_required(VERSION 3.20)

add_library(error_handler STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/error_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/error_handler.h
)

target_include_directories(error_handler PUBLIC
     ${CMAKE_CURRENT_SOURCE_DIR}
)

#dependencies
target_link_libraries(error_handler PUBLIC
    HAL_Driver
    logger
)

message(STATUS "error_handler library is configured")
