project(${PROJECT_NAME}_examples)

function(add_example NAME)
    set(F_ARGS CONSOLE_APP)
    set(S_ARGS )
    set(M_ARGS LIBS FOLDER)
    cmake_parse_arguments(ARGS "${F_ARGS}" "${S_ARGS}" "${M_ARGS}" ${ARGN})
    set(FOLDER_PATH)
    set(NAME_PREFIX)
    if(ARGS_FOLDER)
        foreach(SUBFOLDER ${ARGS_FOLDER})
            set(FOLDER_PATH "${FOLDER_PATH}${SUBFOLDER}/")
            set(NAME_PREFIX "${NAME_PREFIX}${SUBFOLDER}_")
        endforeach()
    endif()
    set(TGT ${PROJECT_NAME}_${NAME_PREFIX}${NAME})
    set(FOLDER_PATH "${FOLDER_PATH}${NAME}")
    add_executable(${TGT} "${FOLDER_PATH}/main.cpp")
    if(ARGS_LIBS)
        target_link_libraries(${TGT} PRIVATE ${ARGS_LIBS})
    endif()
    if(NOT ARGS_CONSOLE_APP)
        if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
            if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                target_link_options(${TGT} PUBLIC -Wl,/SUBSYSTEM:WINDOWS,/ENTRY:mainCRTStartup)
            elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                target_link_options(${TGT} PUBLIC /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
            endif()
        endif()
    endif()
endfunction()

find_package(glad CONFIG REQUIRED)

find_path(STB_INCLUDE_DIRS "stb.h")
add_library(stb_interface INTERFACE)
target_include_directories(stb_interface INTERFACE ${STB_INCLUDE_DIRS})
add_library(stb::stb ALIAS stb_interface)

add_example(1_simple
    CONSOLE_APP
    FOLDER 1_getting_started 1_window
    LIBS
        cuiui::cuiui
)
add_example(gl
    CONSOLE_APP
    FOLDER 1_getting_started 1_window 2_clear
    LIBS
        cuiui::cuiui
        coel::coel
        glad::glad
)
add_example(vk
    CONSOLE_APP
    FOLDER 1_getting_started 1_window 2_clear
    LIBS
        cuiui::cuiui
        coel::coel
)
add_example(gl
    CONSOLE_APP
    FOLDER 1_getting_started 2_drawing 1_simple_triangle
    LIBS
        cuiui::cuiui
        coel::coel
        glad::glad
)
add_example(vk
    CONSOLE_APP
    FOLDER 1_getting_started 2_drawing 1_simple_triangle
    LIBS
        cuiui::cuiui
        coel::coel
)
add_example(gl
    CONSOLE_APP
    FOLDER 1_getting_started 2_drawing 2_textured_quad
    LIBS
        cuiui::cuiui
        coel::coel
        glad::glad
        stb::stb
)
add_example(vk
    CONSOLE_APP
    FOLDER 1_getting_started 2_drawing 2_textured_quad
    LIBS
        cuiui::cuiui
        coel::coel
        stb::stb
)

add_example(glvk
    CONSOLE_APP
    FOLDER misc
    LIBS
        coel::coel
        cuiui::cuiui
        glad::glad
)
add_example(raw_win32
    CONSOLE_APP
    FOLDER misc
    CONSOLE_APP
    LIBS
        coel::coel
        cuiui::cuiui
)
