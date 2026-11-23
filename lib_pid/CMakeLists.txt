cmake_minimum_required(VERSION 3.20)

add_library(pid STATIC)

target_sources(pid PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pid.hpp
)
target_include_directories(pid PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
message(STATUS "pid library done")
