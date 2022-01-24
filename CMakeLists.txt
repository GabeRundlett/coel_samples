cmake_minimum_required(VERSION 3.21)

include(cmake/warnings.cmake)

project(coel_test)

# find_package(coel CONFIG REQUIRED)
# find_package(cuiui CONFIG REQUIRED)

set(BUILD_SHARED_LIBS true)

add_subdirectory(ext/coel)
add_subdirectory(ext/cuiui)

set_project_warnings(coel PUBLIC)
set_project_warnings(cuiui INTERFACE)

add_subdirectory(examples)
