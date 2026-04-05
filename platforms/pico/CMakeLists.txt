# Set project name
set(PROJECT DECtalkMini.elf)

# Specify minimum required CMake version
cmake_minimum_required(VERSION 3.12)

# Import the Pico SDK and other needed dependencies
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

# Define the project and its supported languages
project(${PROJECT} C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

FILE(GLOB dtkSrc ../../src/*.c)
list(REMOVE_ITEM dtkSrc "${CMAKE_CURRENT_LIST_DIR}/../../src/main.c")

# Add executable and list all source files
add_executable(${PROJECT}
    ${dtkSrc}
    main.c
)

# Define compile-time configuration options
target_compile_definitions(${PROJECT} PRIVATE
_REENTRANT=1
NOMME=1
LTSSIM=1
TTSSIM=1
ANSI=1
BLD_DECTALK_DLL=1
ENGLISH=1
ENGLISH_US=1
ACCESS32=1
TYPING_MODE=1
ACNA=1
DISABLE_AUDIO=1
SINGLE_THREADED=1
NO_FILESYSTEM=1
__unix__=1
PICO_AUDIO_I2S_MONO_OUTPUT=1 # mono output on I2S
PICO_AUDIO_I2S_MONO_INPUT=1 # required for output
AUDIO_HALF_FREQ=1 # fix frequency for PWM
HANG_FIXES=1
)

# Link necessary libraries
target_link_libraries(${PROJECT}
    pico_stdlib
    pico_multicore
    pico_util
    pico_audio_i2s
    pico_audio_pwm
    pico_unique_id
    hardware_pwm
)

# Add extra outputs (e.g., UF2 file for bootloader)
pico_add_extra_outputs(${PROJECT})

# USB serial for debug output; UART is used directly by core 1 for TTS input
pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)

# Specify include directories for the project

target_include_directories(${PROJECT} PUBLIC ../../include ${CMAKE_CURRENT_LIST_DIR})
