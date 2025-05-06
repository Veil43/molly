cmake_minimum_required(VERSION 3.20)

project(molly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE_DEBUG ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option (INCLUDE_GRAPHICS "Build with the grpahics portion" ON)

if (INCLUDE_GRAPHICS)

add_executable(molly src/main.cc 
    third-party/glad/src/glad.c
    src/glfw_platform.cc
    src/molly.cc
    src/mesh.cc
    src/camera.cc
    src/shader.cc
    src/texture.cc
    src/utils.cc
    src/glfw_utils.cc
    src/asset_registry.cc
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
target_include_directories(molly PRIVATE third-party/tiny_gltf)

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


# TESTING --------------------------------------------
# Add Catch2
add_subdirectory(third-party/Catch2)
include(CTest)
include(Catch)

# create test target
add_executable(test_asset_registry
    tests/test_asset_registry.cc
    src/asset_registry.cc
    src/textfile.cc
    src/context.cc) 
    
# Link libs
target_link_libraries(test_asset_registry PRIVATE Catch2::Catch2WithMain)
    
# Include headers
target_include_directories(test_asset_registry PRIVATE third-party/glad/include)
target_include_directories(test_asset_registry PRIVATE third-party/stb)
target_include_directories(test_asset_registry PRIVATE third-party/tiny_gltf)
target_include_directories(test_asset_registry PRIVATE third-party/glm)
target_include_directories(test_asset_registry PRIVATE src)
    
add_executable(test_text_file
    tests/test_textfile.cc
    src/textfile.cc)

target_include_directories(test_text_file PRIVATE src)
target_link_libraries(test_text_file PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(test_text_file)
catch_discover_tests(test_asset_registry)
enable_testing()