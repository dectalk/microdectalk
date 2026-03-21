set(TARGET_TRIPLE "aarch64-windows-gnu")
set(CMAKE_SYSTEM_NAME "Windows")
set(CMAKE_SYSTEM_PROCESSOR "aarch64")

set(OPUS_DISABLE_INTRINSICS ON)

include("${CMAKE_CURRENT_LIST_DIR}/zig-base.cmake")
