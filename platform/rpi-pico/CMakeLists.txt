# Set project name
set(PROJECT hello_usb_cpp)

# Specify minimum required CMake version
cmake_minimum_required(VERSION 3.12)

# Import the Pico SDK and other needed dependencies
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

# Define the project and its supported languages
project(${PROJECT} C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

FILE(GLOB dtkSrc ../../src*.c)

# Add executable and list all source files
add_executable(${PROJECT}
    ${dtkSrc}
    src/main.c
)

# Define compile-time configuration options
target_compile_definitions(${PROJECT} PRIVATE
    PICO_AUDIO_I2S_MONO_INPUT=1
    USE_AUDIO_I2S=1
    PICO_AUDIO_I2S_DATA_PIN=28
    PICO_AUDIO_I2S_CLOCK_PIN_BASE=26
    SIMULATOR=1
    SINGLE_THREADED=1
    HACK_ALERT=1
    FULL_LANGUAGE_SUPPORT=1
    FULL_LTS=1
    NO_INDEXES=1
    DIRECT_LTS_INPUT=1
    ACNA=1
    HOMOGRAPHS=1
    MATH_MODE=1
    DIVIDED_LTS_RULES=1
    OUTPUT_FILE=1
    __linux__=1
    SINGLE_LANGUAGE=1
    Multiple_Sample_Rates=1
)

# Link necessary libraries
target_link_libraries(${PROJECT}
    pico_stdlib
    pico_audio_i2s
    pico_audio_pwm
    pico_unique_id
    hardware_pwm
)

# Add extra outputs (e.g., UF2 file for bootloader)
pico_add_extra_outputs(${PROJECT})

# Enable USB and UART output
pico_enable_stdio_usb(${PROJECT} 1)  # Enable USB serial
pico_enable_stdio_uart(${PROJECT} 0) # Disable UART serial

# Specify include directories for the project

target_include_directories(${PROJECT} PUBLIC ../../include ${CMAKE_CURRENT_LIST_DIR})
