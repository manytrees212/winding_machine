# Master CMake for all displays
cmake_minimum_required(VERSION 3.20)

add_subdirectory(stm32_st7789)
add_subdirectory(stm32_1602)

message(STATUS "Display libraries configured")
