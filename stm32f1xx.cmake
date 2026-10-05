# cmake/stm32f1xx.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)

# Set compilers directly
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

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

add_compile_definitions(STM32F10X_MD STM32F103xB)

option(USE_HAL_DRIVER "Use STM32 HAL Library" ON)
if(USE_HAL_DRIVER)
    add_compile_definitions(
        USE_HAL_DRIVER
    )
    message(STATUS "Library selected: HAL (USE_HAL_DRIVER defined)")
else()
    message(STATUS "HAL not selected selected")
endif()

message(STATUS "Toolchain loaded: STM32F1 (Cortex-M3)")
message(STATUS "MCU flags: ${MCU_FLAGS}")
