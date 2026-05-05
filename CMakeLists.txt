cmake_minimum_required(VERSION 3.25)
project(dectalkmini)

file(GLOB SOURCES src/*.c)

add_library(dtc STATIC ${SOURCES})

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
