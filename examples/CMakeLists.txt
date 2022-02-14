project(${PROJECT_NAME}_examples)

function(add_example)
    set(F_ARGS CONSOLE_APP)
    set(S_ARGS )
    set(M_ARGS LIBS FOLDER)
    cmake_parse_arguments(ARGS "${F_ARGS}" "${S_ARGS}" "${M_ARGS}" ${ARGN})
    set(FOLDER_PATH)
    set(NAME_PREFIX)
    if(ARGS_FOLDER)
        foreach(SUBFOLDER ${ARGS_FOLDER})
            set(FOLDER_PATH "${FOLDER_PATH}${SUBFOLDER}/")
            set(NAME_PREFIX "${NAME_PREFIX}_${SUBFOLDER}")
        endforeach()
    endif()
    set(TGT ${PROJECT_NAME}${NAME_PREFIX})
    set(FOLDER_PATH "${FOLDER_PATH}")
    add_executable(${TGT} "${FOLDER_PATH}/main.cpp")
    if(ARGS_LIBS)
        target_link_libraries(${TGT} PRIVATE ${ARGS_LIBS})
    endif()
    target_include_directories(${TGT} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
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

find_package(assimp CONFIG REQUIRED)

find_package(glad CONFIG REQUIRED)

find_package(glm CONFIG REQUIRED)

find_path(STB_INCLUDE_DIRS "stb.h")
add_library(stb_interface INTERFACE)
target_include_directories(stb_interface INTERFACE ${STB_INCLUDE_DIRS})
add_library(stb::stb ALIAS stb_interface)

add_example(FOLDER 1_getting_started 1_window 1_simple
    CONSOLE_APP
    LIBS
        cuiui::cuiui
)
add_example(FOLDER 1_getting_started 1_window 2_clear gl
    CONSOLE_APP
    LIBS
        cuiui::cuiui
        coel::coel
        glad::glad
)
add_example(FOLDER 1_getting_started 1_window 2_clear vk
    CONSOLE_APP
    LIBS
        cuiui::cuiui
        coel::coel
)
add_example(FOLDER 1_getting_started 2_drawing 1_simple_triangle gl
    CONSOLE_APP
    LIBS
        cuiui::cuiui
        coel::coel
        glad::glad
)
add_example(FOLDER 1_getting_started 2_drawing 1_simple_triangle vk
    CONSOLE_APP
    LIBS
        cuiui::cuiui
        coel::coel
)
add_example(FOLDER 1_getting_started 2_drawing 2_textured_quad gl
    CONSOLE_APP
    LIBS
        cuiui::cuiui
        coel::coel
        glad::glad
        stb::stb
)
add_example(FOLDER 1_getting_started 2_drawing 2_textured_quad vk
    CONSOLE_APP
    LIBS
        cuiui::cuiui
        coel::coel
        stb::stb
)
add_example(FOLDER 1_getting_started 2_drawing 3_shaders
    CONSOLE_APP
    LIBS
        cuiui::cuiui
        coel::coel
        glad::glad
        stb::stb
        assimp::assimp
        glm::glm
)

add_example(FOLDER misc glvk
    CONSOLE_APP
    LIBS
        coel::coel
        cuiui::cuiui
        glad::glad
)
add_example(FOLDER misc raw_win32
    CONSOLE_APP
    CONSOLE_APP
    LIBS
        coel::coel
        cuiui::cuiui
)
add_example(FOLDER misc voxels
    CONSOLE_APP
    LIBS
        cuiui::cuiui
        coel::coel
        glad::glad
        stb::stb
)

add_example(FOLDER misc docking
    CONSOLE_APP
    LIBS
        cuiui::cuiui
        coel::coel
        glad::glad
        stb::stb
        assimp::assimp
)
