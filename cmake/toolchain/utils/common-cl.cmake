
list(APPEND CMAKE_C_STANDARD_INCLUDE_DIRECTORIES $ENV{INCLUDE})
list(REMOVE_DUPLICATES CMAKE_C_STANDARD_INCLUDE_DIRECTORIES)
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_STANDARD_INCLUDE_DIRECTORIES} CACHE STRING "")

list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES $ENV{INCLUDE})
list(REMOVE_DUPLICATES CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES} CACHE STRING "")

list(APPEND LINK_DIRECTORIES $ENV{LIB} $ENV{LIBPATH})
list(REMOVE_DUPLICATES LINK_DIRECTORIES)
set(LINK_DIRECTORIES ${LINK_DIRECTORIES} CACHE STRING "")
link_directories(BEFORE ${LINK_DIRECTORIES})
