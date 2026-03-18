# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.0)

project("epsonapi")

set(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../..")
file(GLOB ENGINE_SOURCES "${REPO_ROOT}/src/*.c")

add_library(${CMAKE_PROJECT_NAME} SHARED
        ${ENGINE_SOURCES}
        native-lib.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        "${REPO_ROOT}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        -D_REENTRANT
        -DNOMME
        -DLTSSIM
        -DTTSSIM
        -DANSI
        -DBLD_DECTALK_DLL
        -DENGLISH
        -DENGLISH_US
        -DACCESS32
        -DTYPING_MODE
        -DOS_SIXTY_FOUR_BIT
        -DACNA
        -DDISABLE_AUDIO
        -DSINGLE_THREADED
        -DNO_FILESYSTEM
        -DHANG_FIXES
)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log)
