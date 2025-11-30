cmake_minimum_required(VERSION 3.31)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/mingw-toolchain.cmake" CACHE PATH "Path to toolchain file")

project(GTFOHax LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Parse version from PropertySheet.props
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/GTFOHax/PropertySheet.props" PROPS_CONTENT)
string(REGEX MATCH "<LibraryVersion>([^<]+)</LibraryVersion>" _ "${PROPS_CONTENT}")
set(LIBRARY_VERSION_NUMBER "${CMAKE_MATCH_1}")

# Dependencies
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)
set(IMGUI_BACKEND_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
)

set(KIERO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/kiero)
set(KIERO_SOURCES ${KIERO_DIR}/kiero.cpp)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/GTFOHax)

# GTFOHax sources
set(GTFOHAX_SOURCES
    ${SRC_DIR}/framework/dllmain.cpp
    ${SRC_DIR}/framework/pch-il2cpp.cpp
    ${SRC_DIR}/framework/helpers.cpp
    ${SRC_DIR}/framework/il2cpp-init.cpp
    ${SRC_DIR}/config/config.cpp
    ${SRC_DIR}/fonts/fonts.cpp
    ${SRC_DIR}/hacks/esp.cpp
    ${SRC_DIR}/hacks/aimbot.cpp
    ${SRC_DIR}/hacks/enemy.cpp
    ${SRC_DIR}/hacks/player.cpp
    ${SRC_DIR}/user/main.cpp
    ${SRC_DIR}/globals.cpp
    ${SRC_DIR}/utils/math.cpp
    ${SRC_DIR}/ui.cpp
    ${SRC_DIR}/InputUtil.cpp
    ${SRC_DIR}/hooks.cpp
    ${SRC_DIR}/menu.cpp
)

add_subdirectory(${KIERO_DIR}/minhook)

# Apply patches to imgui
file(GLOB IMGUI_PATCHES "${CMAKE_CURRENT_SOURCE_DIR}/patches/*.patch")
foreach(PATCH_FILE ${IMGUI_PATCHES})
    get_filename_component(PATCH_NAME ${PATCH_FILE} NAME)

    execute_process(
        COMMAND git apply --check ${PATCH_FILE}
        WORKING_DIRECTORY ${IMGUI_DIR}
        RESULT_VARIABLE PATCH_CHECK_RESULT
        OUTPUT_VARIABLE PATCH_CHECK_OUTPUT
        ERROR_VARIABLE PATCH_CHECK_ERROR
    )

    if(PATCH_CHECK_RESULT EQUAL 0)
        execute_process(
            COMMAND git apply ${PATCH_FILE}
            WORKING_DIRECTORY ${IMGUI_DIR}
            RESULT_VARIABLE PATCH_RESULT
            OUTPUT_VARIABLE PATCH_OUTPUT
            ERROR_VARIABLE PATCH_ERROR
        )
        if(NOT PATCH_RESULT EQUAL 0)
            message(WARNING "Error applying patch ${PATCH_NAME}:\n${PATCH_OUTPUT}\n${PATCH_ERROR}\nIf already applied, this can be ignored.")
        else()
            message(STATUS "Applied patch ${PATCH_NAME} successfully")
        endif()
    else()
        message(STATUS "Patch ${PATCH_NAME} already applied or cannot be applied:\n${PATCH_CHECK_OUTPUT}\n${PATCH_CHECK_ERROR}")
    endif()
endforeach()

# GTFOHax DLL target
add_library(GTFOHaxDLL SHARED
    ${GTFOHAX_SOURCES}
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${KIERO_SOURCES}
)

target_include_directories(GTFOHaxDLL PRIVATE
    ${SRC_DIR}
    ${SRC_DIR}/appdata/gcc
    ${SRC_DIR}/config
    ${SRC_DIR}/fonts
    ${SRC_DIR}/framework
    ${SRC_DIR}/hacks
    ${SRC_DIR}/user
    ${KIERO_DIR}/minhook/include
    ${KIERO_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_compile_options(GTFOHaxDLL PRIVATE
    -static-libgcc
    -static-libstdc++
    -fpermissive
    -Wno-narrowing
    $<$<CONFIG:Debug>:-g -ggdb -gdwarf-4 -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls>
    $<$<CONFIG:Release>:-O2 -s>
)

target_compile_definitions(GTFOHaxDLL PRIVATE
    _USE_MATH_DEFINES
    $<$<CONFIG:Debug>:_DEBUG _CONSOLE>
    $<$<CONFIG:Debug>:LIBRARY_VERSION=R"\(${LIBRARY_VERSION_NUMBER}\ DEBUG\ BUILD\)">
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Release>:LIBRARY_VERSION=R"\(${LIBRARY_VERSION_NUMBER}\)">
)

target_link_libraries(GTFOHaxDLL PRIVATE
    minhook
    d3d11
    dxgi
    d3dcompiler
    dwmapi
)

set_target_properties(GTFOHaxDLL PROPERTIES
    OUTPUT_NAME "$<IF:$<CONFIG:Release>,GTFOHax-v${LIBRARY_VERSION_NUMBER},GTFOHax>"
    PREFIX ""
    SUFFIX ".dll"
)

target_link_options(GTFOHaxDLL PRIVATE
    -Wl,--subsystem,windows
    -Wl,--enable-runtime-pseudo-reloc
    -Wl,--exclude-libs,ALL
    -static
    -pthread
    $<$<CONFIG:Release>:-s>
)

# Injector executable
add_executable(injector ${CMAKE_CURRENT_SOURCE_DIR}/injector.cpp)

target_compile_options(injector PRIVATE
    -static-libgcc
    -static-libstdc++
    -municode
    $<$<CONFIG:Debug>:-g -ggdb -gdwarf-4 -O0 -fno-omit-frame-pointer>
    $<$<CONFIG:Release>:-O2 -s>
)

target_link_libraries(injector PRIVATE
    shlwapi
)

target_link_options(injector PRIVATE
    -Wl,--subsystem,console
    -static
    -municode
    $<$<CONFIG:Release>:-s>
)

set_target_properties(injector PROPERTIES
    OUTPUT_NAME "injector"
    SUFFIX ".exe"
)
