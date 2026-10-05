cmake_minimum_required(VERSION 3.20)
project(winding_machine C CXX ASM)

# ============================================
# 0. BASE PATH DEFINITIONS
# ============================================

set(STM32_BASE ${CMAKE_CURRENT_SOURCE_DIR})

set(CMSIS_CORE_PATH ${STM32_BASE}/CMSIS/Core/CMSIS/Core)
set(CMSIS_DEVICE_PATH ${STM32_BASE}/CMSIS/Device)
set(HAL_DRIVER_PATH ${STM32_BASE}/HAL_Driver)

#Logger Library path
set(LOGGER_PATH ${STM32_BASE}/lib_logger)

#Error_handler library path
set(ERROR_HANDLER_PATH ${STM32_BASE}/lib_error_handler)

#Display library path
set(LIB_DISPLAYS_PATH ${STM32_BASE}/lib_displays)

#EEPROM library path
set(EEPROM_PATH ${STM32_BASE}/lib_eeprom)

# ============================================
# 1. ADD LIBRARIES
# ============================================

add_library(HAL_Driver INTERFACE)
target_include_directories(HAL_Driver INTERFACE
    ${HAL_DRIVER_PATH}/Inc
    ${CMSIS_CORE_PATH}/Include
    ${CMSIS_DEVICE_PATH}/Include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_compile_definitions(HAL_Driver INTERFACE
    USE_HAL_DRIVER
    STM32F103xB
)

add_library(stm32_hardware_flags INTERFACE)
target_compile_options(stm32_hardware_flags INTERFACE
    -fdata-sections
    -ffunction-sections
    -Wall
    -Wextra
    -Wpedantic
)

target_link_options(stm32_hardware_flags INTERFACE
    --specs=nosys.specs
    --specs=nano.specs
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,--no-warn-mismatch
)

add_subdirectory(${LOGGER_PATH} ${CMAKE_CURRENT_BINARY_DIR}/logger)
add_subdirectory(${ERROR_HANDLER_PATH} ${CMAKE_CURRENT_BINARY_DIR}/error_handler)
add_subdirectory(${LIB_DISPLAYS_PATH}/stm32_st7789 ${CMAKE_CURRENT_BINARY_DIR}/stm32_st7789)
add_subdirectory(${EEPROM_PATH} ${CMAKE_CURRENT_BINARY_DIR}/eeprom)

# ============================================
# 2. SOURCE & HEADER FILES
# ============================================

set(SOURCES
    src/main.cpp
    src/cpp_support.cpp
    src/chip_init.cpp
    src/eeprom_data_init.cpp
    src/gpio.c
    src/sysclock_init.c
    src/exti.c
    src/uart_init.c
    src/spi_init.c
    src/tim.c
    src/syscalls.c
    src/display_st7789.c
    src/flash_data_controller.cpp
    src/button_controller.cpp
    src/encoder.cpp
    src/pid.cpp
    src/uart_cmd_parser.cpp
    src/winding_machine.cpp
    ${CMSIS_DEVICE_PATH}/Source/Templates/system_stm32f1xx.c
    ${CMSIS_DEVICE_PATH}/Source/Templates/gcc/startup_stm32f103xb.s
)

# HAL source files needed for current task
if(USE_HAL_DRIVER)
    list(APPEND SOURCES
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_pwr.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_flash.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_flash_ex.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_gpio.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_rcc.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_cortex.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_uart.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_dma.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_spi.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_tim.c
        ${HAL_DRIVER_PATH}/Src/stm32f1xx_hal_tim_ex.c
    )
    # Build Type Logic
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type" FORCE)
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_definitions(USE_HAL_DRIVER USE_FULL_ASSERT)
        message(STATUS ">> Debug Build: USE_FULL_ASSERT is ENABLED")
    else()
        add_compile_definitions(USE_HAL_DRIVER)
        message(STATUS ">> Release Build: USE_FULL_ASSERT is DISABLED")
    endif()

    message(STATUS "HAL sources added")
endif()

# ============================================
# 3. CREATE EXECUTABLE & INCLUDES
# ============================================
add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE
    #HAL_DRIVER
    logger
    error_handler
    stm32_st7789 #spi tft_display
    eeprom
    stm32_hardware_flags
)

# ============================================
# 4. COMPILER OPTIONS
# ============================================

set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU>:-mcpu=cortex-m3>
    $<$<COMPILE_LANG_AND_ID:C,GNU>:-mthumb>
    $<$<COMPILE_LANG_AND_ID:C,GNU>:-mfloat-abi=soft>

    $<$<COMPILE_LANGUAGE:CXX>:-std=c++20>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-mcpu=cortex-m3>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-mthumb>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-mfloat-abi=soft>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-unwind-tables>
    -O0
    -Wshadow
    -Wdouble-promotion
    -g
)

# ============================================
# 5. LINKER OPTIONS
# ============================================
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld
    -Wl,-Map=${PROJECT_NAME}.map
)

# ============================================
# 6. POST-BUILD & UTILITIES (Flash/Size/Erase)
# ============================================
# Generate binary and hex files
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
    COMMENT "Generating binary and hex files..."
)

# Flash target via ST-Link
find_program(OPENOCD openocd)
if(NOT OPENOCD)
    message(WARNING "OpenOCD not found. Flash target disabled.")
else()
    # Flash with OpenOCD
    add_custom_target(flash
        COMMAND ${OPENOCD} -f interface/stlink.cfg -f target/stm32f1x.cfg
                -c "init"
                -c "reset halt"
                -c "flash write_image erase ${PROJECT_NAME}.elf"
                -c "verify_image ${PROJECT_NAME}.elf"
                -c "reset run"
                -c "shutdown"
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Flashing ${PROJECT_NAME}.elf via OpenOCD..."
    )

    # Flash directly by ST-Link
    add_custom_target(flash_direct
    COMMAND st-flash --reset write ${PROJECT_NAME}.bin 0x8000000
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing Blue Pill via ST-Link..."
    )
    
    # Flash and halt (for debugging)
    add_custom_target(flash-halt
        COMMAND ${OPENOCD} -f interface/stlink.cfg -f target/stm32f1x.cfg
                -c "program ${PROJECT_NAME}.elf verify"
                -c "reset halt"
                -c "exit"
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Flashing and halting for debugging..."
    )
    
    # Erase flash
    add_custom_target(erase
        COMMAND ${OPENOCD} -f interface/stlink.cfg -f target/stm32f1x.cfg
                -c "init"
                -c "stm32f1x mass_erase 0"
                -c "exit"
        COMMENT "Erasing entire flash..."
    )
endif()

# Make ST-Link flashing available as a runnable target
# set_target_properties(flash PROPERTIES
#    FOLDER "qtc_runnable"
#)

# Flash target via bootloader
find_program(STM32FLASH stm32flash)
if(NOT STM32FLASH)
    message(WARNING "stm32flash not found. UART bootloader flash targets disabled.")
else()
    if(NOT SERIAL_PORT) # if there is no -DSERIAL_PORT=/dev/ttyUSB0
        if(EXISTS "/dev/ttyUSB0")
            set(SERIAL_PORT "/dev/ttyUSB0" CACHE STRING "Serial port for UART flashing")
        elseif(EXISTS "/dev/ttyACM0")
            set(SERIAL_PORT "/dev/ttyACM0" CACHE STRING "Serial port for UART flashing")
        else()
            message(WARNING "Override with: cmake -DSERIAL_PORT=/dev/ttyUSB0 ..")
        endif()
    endif()

    # Basic UART flash (default baud rate 57600)
    add_custom_target(flash_uart
        COMMAND ${STM32FLASH} -w ${PROJECT_NAME}.bin -v -g 0x8000000 ${SERIAL_PORT}
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Flashing via internal bootloader over UART (${SERIAL_PORT})..."
    )

    # Fast UART flash (higher baud rate - up to 230400 works on Blue Pill)
    add_custom_target(flash_uart_fast
        COMMAND ${STM32FLASH} -b 115200 -w ${PROJECT_NAME}.bin -v -g 0x8000000 ${SERIAL_PORT}
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Flashing via internal bootloader @ 115200 baud on ${SERIAL_PORT}..."
    )

    # Very fast UART flash (maximum supported baud rate)
    add_custom_target(flash_uart_ultra
        COMMAND ${STM32FLASH} -b 230400 -w ${PROJECT_NAME}.bin -v -g 0x8000000 ${SERIAL_PORT}
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Flashing via internal bootloader @ 230400 baud on ${SERIAL_PORT}..."
    )

    # Erase via UART bootloader
    add_custom_target(erase_uart
        COMMAND ${STM32FLASH} -e ${SERIAL_PORT}
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Erasing flash via internal bootloader on ${SERIAL_PORT}..."
    )

    # Read back flash via UART (save to file)
    add_custom_target(read_flash_uart
        COMMAND ${STM32FLASH} -r backup_${PROJECT_NAME}.bin -S 0x8000000:0x8002000 ${SERIAL_PORT}
        COMMENT "Reading flash (first 8KB) via internal bootloader..."
    )

    # Interactive UART terminal (for debugging bootloader communication)
    add_custom_target(uart_terminal
        COMMAND ${STM32FLASH} -i ${SERIAL_PORT}
        COMMENT "Interactive UART terminal on ${SERIAL_PORT} (Ctrl+C to exit)..."
    )
endif()

# Make UART bootloader flashing available as a runnable target
# set_target_properties(flash_uart PROPERTIES
#   FOLDER "qtc_runnable"
#)

# ============================================
# DIAGNOSTICS
# ============================================
message(STATUS "[Diag] Querying final compiler settings...")

# Check local options defined directly on the .elf target
get_target_property(LOCAL_OPTS ${PROJECT_NAME}.elf COMPILE_OPTIONS)
if(LOCAL_OPTS)
    message(STATUS "-- Local Compile Options:")
    foreach(flag IN LISTS LOCAL_OPTS)
        # Print raw GEs so we can see exactly what was requested
        message(STATUS "  -> ${flag}")
    endforeach()
else()
    message(STATUS "-- Local Compile Options: <None>")
endif()

# Check if hardware flags were successfully passed down
get_target_property(LINKED_LIBS ${PROJECT_NAME}.elf LINK_LIBRARIES)
if(LINKED_LIBS)
    message(STATUS "-- Linked Libraries:")
    foreach(lib IN LISTS LINKED_LIBS)
        # Only process our hardware lib
        if(TARGET ${lib})
            get_target_property(INT_OPTS ${lib} INTERFACE_COMPILE_OPTIONS)
            if(INT_OPTS AND NOT INT_OPTS STREQUAL "INT_OPTS-NOTFOUND")
                message(STATUS "  -> [${lib}]")
                foreach(flag IN LISTS INT_OPTS)
                    message(STATUS "     | ${flag}")
                endforeach()
            endif()
        endif()
    endforeach()
endif()

# Evaluate them into actual strings using generator expressions
# This simulates what GCC actually sees
set(CMD_LINE "$<TARGET_PROPERTY:${PROJECT_NAME}.elf,COMPILE_OPTIONS>")
separate_arguments(EVAL_CMD UNIX_COMMAND "${CMD_LINE}")

message(STATUS "-- Final Evaluated Command Line Flags (for build log):")
# We use VERBOSE here because separate_arguments struggles with complex GEs
# A better way is to just check make VERBOSE=1 during compilation
foreach(flag IN LISTS EVAL_CMD)
    # Only strip empty literals, keep real content
    if(NOT flag STREQUAL "" AND NOT flag MATCHES "^\\$<.*>$")
        message(STATUS "  | ${flag}")
    endif()
endforeach()

message(STATUS "========================================")
message(STATUS "Build Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  MCU: STM32F103xB (Cortex-M3)")
message(STATUS "  Library: HAL: ${USE_HAL_DRIVER}")
message(STATUS "  Source files: ${SOURCES}")

if(STM32FLASH)
    message(STATUS "  UART Bootloader: ENABLED (${STM32FLASH})")
    message(STATUS "  Serial Port: ${SERIAL_PORT}")
else()
    message(STATUS "  UART Bootloader: DISABLED (stm32flash not found)")
endif()

message(STATUS "========================================")
