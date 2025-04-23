#include <iostream>
#include "glfw_platform.h"
#include "config.h"
#include "rendertoy.h"

PLATFROM_DECL_KEY_CALLBACK(platformDefaultKeyCallback) {
    platformCMDLog("WARNING: Currently using <platformDefaultKeyCallback>. Please provide a key callback.");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

PLATFORM_DECL_FRAMEBUFFER_SIZE_CALLBACK(platformDefaultFramebufferSizeCallback) {
    glViewport(0, 0, width, height);
}

// GLOBAL VARIABLES FOR THE PLATFORM LAYER
static GLFWKeyCallBackType* platformKeyCallback = platformDefaultKeyCallback;

GLFWPlatformWindow platformCreateWindow() {
    
    GLFWPlatformWindow window = {};
    
    if (!glfwInit()) {
        platformCMDLog(platformGetError("ERROR::GLFW::INIT").c_str());
        window.window_handle = nullptr;
        window.window_title = "ERROR::GLFW::INIT";
        platformCloseWindow(window);
        return window;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kOpenGLMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kOpenGLMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, kOpenGLProfile);

    window.window_title = kWindowTitle;
    window.window_width = static_cast<i32>(kWindowWidth);
    window.window_height = static_cast<i32>(kWindowHeight);
    window.window_handle = glfwCreateWindow(
        window.window_width,
        window.window_height,
        window.window_title.c_str(),
        NULL, NULL
    );

    if (!window.window_handle) {
        platformCMDLog(platformGetError("ERROR::GLFW::WINDOW").c_str());
        window.window_handle = nullptr;
        window.window_title = "ERROR::GLFW::WINDOW";
        platformCloseWindow(window);
        return window;
    }
    glfwMakeContextCurrent(window.window_handle);

    if (!gladLoadGL()) {
        platformCloseWindow(window);
        return window;
    }

    // Pass (potentially) user defined callbacks to glfw
    glfwSetKeyCallback(window.window_handle, platformKeyCallback);
    glfwSetFramebufferSizeCallback(window.window_handle, platformDefaultFramebufferSizeCallback);
    // Call application startup code
    renderToyOnStartupCall(window.window_width/window.window_height);

    return window;
}

void platformRunWindowLoop(GLFWPlatformWindow window) {
    static f32 s_delta_time = 0.0f;
    
    while(!glfwWindowShouldClose(window.window_handle)) {
        glfwPollEvents();
        
        // Call application looping code
        renderToyRenderLoop(s_delta_time);

        glfwSwapBuffers(window.window_handle);
    }
}

void platformCloseWindow(GLFWPlatformWindow window) {
    if (window.window_handle != nullptr) {
        glfwDestroyWindow(window.window_handle);
    }
    glfwTerminate();
}

std::string platformGetError(const std::string& error) {
    const char* description;
    std::string error_string = "";
    if (glfwGetError(&description)) {
        error_string = error + " " + description;
    }
    return error_string;
}

void platformSetCallback(GLFWKeyCallBackType* key_callback) {
    platformKeyCallback = key_callback != nullptr ? key_callback : platformKeyCallback;
}

void platformCMDLog(const char* message) {
    std::cerr << message << std::endl;
}