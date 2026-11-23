set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set compilers
set(CMAKE_C_COMPILER /usr/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER /usr/bin/arm-none-eabi-gcc)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Find additional tools
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
find_program(CMAKE_OBJDUMP arm-none-eabi-objdump)
find_program(CMAKE_SIZE arm-none-eabi-size)

# Base MCU flags
set(MCU_FLAGS "-mcpu=cortex-m3 -mthumb -mfloat-abi=soft")

# Compiler flags
set(CMAKE_C_FLAGS_INIT "${MCU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${MCU_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

message(STATUS "Toolchain loaded: STM32F1 (Cortex-M3)")
message(STATUS "MCU flags: ${MCU_FLAGS}")