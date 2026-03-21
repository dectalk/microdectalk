cmake_minimum_required(VERSION 3.12)

project(libdtc LANGUAGES C)

set(JDK_8_VERSION "jdk8u462-ga")
set(OPUS_VERSION "v1.5.2")

if (WIN32)
    set(JNI_MD_URL "https://raw.githubusercontent.com/openjdk/jdk8u/refs/tags/${JDK_8_VERSION}/jdk/src/windows/javavm/export/jni_md.h")
elseif (APPLE)
    set(JNI_MD_URL "https://raw.githubusercontent.com/openjdk/jdk8u/refs/tags/${JDK_8_VERSION}/jdk/src/macosx/javavm/export/jni_md.h")
elseif (UNIX)
    set(JNI_MD_URL "https://raw.githubusercontent.com/openjdk/jdk8u/refs/tags/${JDK_8_VERSION}/jdk/src/solaris/javavm/export/jni_md.h")
else ()
    message(WARNING "Unknown OS, not compiling jni_md.h")
endif ()

file(DOWNLOAD
        "https://raw.githubusercontent.com/openjdk/jdk8u/refs/tags/${JDK_8_VERSION}/jdk/src/share/javavm/export/jni.h"
        ${CMAKE_BINARY_DIR}/jni_headers/jni.h
)

if (DEFINED JNI_MD_URL)
    file(DOWNLOAD
            ${JNI_MD_URL}
            ${CMAKE_BINARY_DIR}/jni_headers/jni_md.h
    )
endif ()

file(GLOB DTC_SOURCES
    "../../../src/*.c"
)
# Windows-only compatibility sources
if(WIN32)
    file(GLOB DTC_WIN32_SOURCES CONFIGURE_DEPENDS
        "../../../mman-win32/*.c"
    )
    list(APPEND DTC_SOURCES ${DTC_WIN32_SOURCES})
endif()

add_library(libdtc SHARED
        ${DTC_SOURCES}
        src/TTSNative.c
)

target_include_directories(libdtc PRIVATE
        ../../../include
        ${CMAKE_BINARY_DIR}/jni_headers
)

target_compile_definitions(libdtc PRIVATE
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
)

if(WIN32)
    # Matches the Premake script, though it is unusual to define __unix__ on Windows.
    target_compile_definitions(libdtc PRIVATE __unix__)
endif()

message(STATUS "C compiler executable: ${CMAKE_C_COMPILER}")
