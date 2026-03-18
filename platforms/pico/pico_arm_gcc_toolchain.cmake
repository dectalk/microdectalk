# Generic ARM bare-metal toolchain for Pico SDK
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Path to your unpacked toolchain
set(TOOLCHAIN_DIR "$ENV{HOME}/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi")

# Compiler paths
set(CMAKE_C_COMPILER   "${TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_DIR}/bin/arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc")

# Sysroot paths
set(CMAKE_SYSROOT "${TOOLCHAIN_DIR}/arm-none-eabi")

# Ensure compiler knows where to find its own headers/libraries
set(CMAKE_C_FLAGS_INIT   "--sysroot=${CMAKE_SYSROOT}")
set(CMAKE_CXX_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")

# Prevent CMake from trying to run test binaries on the host
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Optional: explicitly set target architecture for Pico
set(CMAKE_C_FLAGS_INIT   "${CMAKE_C_FLAGS_INIT} -mcpu=cortex-m0plus -mthumb")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -mcpu=cortex-m0plus -mthumb")
