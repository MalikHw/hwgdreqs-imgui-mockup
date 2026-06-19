cmake_minimum_required(VERSION 3.16)
project(HwGDReqs)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT DEFINED APP_VERSION)
    set(APP_VERSION "dev")
endif()
add_compile_definitions(APP_VERSION="${APP_VERSION}")

find_package(SDL2 REQUIRED)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/extern/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl2.cpp
)

# Main executable
add_executable(HwGDReqs WIN32 main.cpp ${IMGUI_SOURCES})

target_include_directories(HwGDReqs PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(HwGDReqs PRIVATE
    ${SDL2_LIBRARIES}
    opengl32
    shell32
    comdlg32
    gdi32
    winmm
    advapi32
    winhttp
    urlmon
)

set_target_properties(HwGDReqs PROPERTIES OUTPUT_NAME "HwGDReqs")