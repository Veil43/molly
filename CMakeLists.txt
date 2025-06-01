cmake_minimum_required(VERSION 3.20)

project(molly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE_DEBUG ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option (INCLUDE_GRAPHICS "Build with the grpahics portion" ON)

if (INCLUDE_GRAPHICS)

add_executable(molly src/main.cc 
    third-party/glad/src/glad.c
    src/molly.cc
    src/camera.cc
    src/shader.cc
    src/utils.cc
    src/model.cc
    src/textfile.cc)

# Define debug symbols
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DMOLLY_DEBUG)
endif()

# Add glad headers
target_include_directories(molly PRIVATE third-party/glad/include)

# Add stb headers
target_include_directories(molly PRIVATE third-party/stb)

# Add tiny_gltf headers
# target_include_directories(molly PRIVATE third-party/tiny_gltf)

# Add the OpenGL dlls
find_package(OpenGL REQUIRED)
target_link_libraries(molly PRIVATE OpenGL::GL)

# Add glfw to the project
add_subdirectory(third-party/glfw)
target_link_libraries(molly PRIVATE glfw)

# Add glm
add_subdirectory(third-party/glm)
target_include_directories(molly PRIVATE third-party/glm)

endif()