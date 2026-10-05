# Pet-project: WindingMachine

- **Host:**        Linux
- **Target MCU:**  stm32f103c8 
- **Library:**     STM32 HAL library
- **Toolchain:**   ARM GCC (`arm-none-eabi-gcc`)

# cloning the repo:
git clone --recurse-submodules https://github.com/manytrees212/winding_machine.git
cd ./winding-machine

# building:

cmake -S . -B ./build -DCMAKE_TOOLCHAIN_FILE=stm32f1xx.cmake
cmake --build ./build

# basic UART flash:

cmake --build ./build --target flash_uart

# erase via UART bootloader:

cmake --build ./build --target erase_uart
