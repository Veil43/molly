cmake_minimum_required(VERSION 3.20)

project(molly)

set(CMAKE_CXX_STANDARD 17)

# symbols
option (INCLUDE_GRAPHICS "Build with the grpahics portion" ON)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DMOLLY_DEBUG)
endif()

if (INCLUDE_GRAPHICS)

# get the source an libraries
find_package(OpenGL REQUIRED)
add_subdirectory(third-party/glfw)
add_subdirectory(third-party/glm)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third-party/imgui)
set(IMGUI_SOURCE
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

add_library(imgui STATIC ${IMGUI_SOURCE})
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    third-party/glfw/include)

#.exe
add_executable(molly src/main.cc 
    third-party/glad/src/glad.c
    src/molly.cc
    src/camera.cc
    src/shader.cc
    src/utils.cc
    src/model.cc
    src/logger.cc
    src/gltf_loader.cc)

# link
target_link_libraries(molly PRIVATE imgui)
target_link_libraries(molly PRIVATE glfw)
target_link_libraries(molly PRIVATE OpenGL::GL)

# headers
target_include_directories(molly PRIVATE third-party/glad/include)
target_include_directories(molly PRIVATE third-party/stb)
target_include_directories(molly PRIVATE third-party/glm)
target_include_directories(molly PRIVATE third-party/tinygltf)

endif()