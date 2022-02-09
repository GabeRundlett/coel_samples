set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang)

set(triple ${CMAKE_SYSTEM_PROCESSOR}-windows-clang)
include(toolchain/utils/common)
include(toolchain/utils/common-clang)
include(toolchain/utils/vcpkg)
