cmake_minimum_required(VERSION 3.25)
project(dectalkmini)

file(GLOB SOURCES src/*.c)

add_library(dtc STATIC ${SOURCES})

option(DECTALKMINI_NO_FILESYSTEM "Disable filesystem" OFF)
option(DECTALKMINI_NO_CHARSET "Disable charset conversion" OFF)
option(DECTALKMINI_EXAMPLES "Build examples" ON)

target_include_directories(dtc PUBLIC include)
target_compile_definitions(
	dtc PRIVATE
	"_REENTRANT"
	"NOMME"
	"LTSSIM"
	"TTSSIM"
	"ANSI"
	"BLD_DECTALK_DLL"
	"ENGLISH"
	"ENGLISH_US"
	"ACCESS32"
	"TYPING_MODE"
	"ACNA"
	"DISABLE_AUDIO"
	"SINGLE_THREADED"
)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(dtc PRIVATE m)
endif()

if(DECTALKMINI_NO_FILESYSTEM)
	target_compile_definitions(dtc PRIVATE "NO_FILESYSTEM")
else()
	if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		target_sources(dtc PRIVATE src/mman-win32/mman.c)
	endif()
endif()

if(DECTALKMINI_NO_CHARSET)
	target_compile_definitions(dtc PRIVATE "NO_CHARSET")
endif()

if(DECTALKMINI_EXAMPLES)
	add_executable(say main.c)
	target_include_directories(say PRIVATE include)
	target_link_libraries(say PRIVATE dtc)
endif()
