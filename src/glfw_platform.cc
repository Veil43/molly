#include <iostream>
#include "glfw_platform.h"
#include "config.h"
#include "rendertoy_services.h"

static PlatformInput 
processInput(GLFWPlatformWindow w, PlatformInput);
static void initializePlatformServices(GLFWwindow* window);

GLFW_DECL_KEY_CALLBACK(platformDefaultKeyCallback) {
    // platformGLFWCMDLog("WARNING: Currently using <platformDefaultKeyCallback>. Please provide a key callback.");
    
    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

GLFW_DECL_FRAMEBUFFER_SIZE_CALLBACK(platformDefaultFramebufferSizeCallback) {
    glViewport(0, 0, width, height);
}

GLFW_DECL_SCROLL_CALLBACK(platformDefaultScrollCallback) {
    renderToyMouseScroll(yoffset);
}

// GLOBAL VARIABLES FOR THE PLATFORM LAYER
static GLFWKeyCallBackType* platformKeyCallback = platformDefaultKeyCallback;
static GLFWScrollCallbackType* scrollCallback = platformDefaultScrollCallback;

// INTERNAL FUNCTIONS ----------------------------------------------------------

static PlatformInput 
processInput(GLFWPlatformWindow w, PlatformInput old_input) {
    static bool first_input = true;

    PlatformInput input = {};
    input.w_key.is_down = (glfwGetKey(w.window_handle, GLFW_KEY_W) == GLFW_PRESS);
    input.w_key.was_down = old_input.w_key.is_down;

    input.a_key.is_down = (glfwGetKey(w.window_handle, GLFW_KEY_A) == GLFW_PRESS);
    input.a_key.was_down = old_input.a_key.is_down;

    input.s_key.is_down = (glfwGetKey(w.window_handle, GLFW_KEY_S) == GLFW_PRESS);
    input.s_key.was_down = old_input.s_key.is_down;

    input.d_key.is_down = (glfwGetKey(w.window_handle, GLFW_KEY_D) == GLFW_PRESS);
    input.d_key.was_down = old_input.d_key.is_down;

    f64 xpos;
    f64 ypos;
    if (first_input) {
        old_input.mouse_x = w.window_width/2;
        old_input.mouse_y = w.window_height/2;
        first_input = false;
    }
    glfwGetCursorPos(w.window_handle, &xpos, &ypos);

    input.mouse_x = static_cast<f32>(xpos);
    input.mouse_prevx = old_input.mouse_x;
    input.mouse_y = static_cast<f32>(ypos);
    input.mouse_prevy = old_input.mouse_y;

    return input;
}

void initializePlatformServices(GLFWwindow* window) {
    platformDisableMouseCursor(window);
    platformEnableMouseCursor(window);
}

// Launch services --------------------------------------------------------------------------
void platformCloseGLFWWindow(GLFWPlatformWindow window) {
    if (window.window_handle != nullptr) {
        glfwDestroyWindow(window.window_handle);
    }
    glfwTerminate();
}

GLFWPlatformWindow platformCreateGLFWWindow() {
    
    GLFWPlatformWindow window = {};
    
    if (!glfwInit()) {
        platformGLFWCMDLog(platformGetGLFWError("ERROR::GLFW::INIT").c_str());
        window.window_handle = nullptr;
        window.window_title = "ERROR::GLFW::INIT";
        platformCloseGLFWWindow(window);
        exit(1);
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
        platformGLFWCMDLog(platformGetGLFWError("ERROR::GLFW::WINDOW").c_str());
        window.window_handle = nullptr;
        window.window_title = "ERROR::GLFW::WINDOW";
        platformCloseGLFWWindow(window);
        exit(1);
    }
    glfwMakeContextCurrent(window.window_handle);

    if (!gladLoadGL()) {
        platformGLFWCMDLog("ERROR::GLAD::LOADGL: Could not load the OpenGL function pointers.");
        window.window_handle = nullptr;
        window.window_title = "ERROR::GLAD::LOADGL";
        platformCloseGLFWWindow(window);
    
        exit(1);
    }

    // Pass (potentially) user defined callbacks to glfw
    glfwSetKeyCallback(window.window_handle, platformKeyCallback);
    glfwSetFramebufferSizeCallback(window.window_handle, platformDefaultFramebufferSizeCallback);
    glfwSetScrollCallback(window.window_handle, platformDefaultScrollCallback);

    initializePlatformServices(window.window_handle);

    // Call application startup code
    renderToyOnStartupCall(window.window_width/window.window_height);
    
    return window;
}

void platformRunGLFWWindowLoop(GLFWPlatformWindow window) {
    static f64 s_delta_time = 0.0f;
    static f64 s_last_time = glfwGetTime();
    
    PlatformInput old_input = {};
    while(!glfwWindowShouldClose(window.window_handle)) {
        platformMeasureTimeElapsed(true);
        glfwPollEvents();
        f64 polling_time = platformMeasureTimeElapsed();
        
        // Call application looping code ----------
        PlatformInput new_input = processInput(window, old_input);
        old_input = new_input;
        renderToyRenderLoop(s_delta_time, new_input);

        glfwSwapBuffers(window.window_handle);
        
        // Timing ----------------------------------
        const f64 new_time = glfwGetTime();
        s_delta_time = (new_time - s_last_time);
        s_last_time = new_time;

#ifdef RENDERTOY_DEBUG
        auto message = std::string("Polling time: ") + std::to_string(polling_time);
        // platformGLFWCMDLog(message.c_str());
#endif

    }
}

std::string platformGetGLFWError(const std::string& error) {
    const char* description;
    std::string error_string = "";
    if (glfwGetError(&description)) {
        error_string = error + " " + description;
    }
    return error_string;
}

/*
Deprecated Does nothing
*/
void platformSetGLFWKeyCallback(GLFWKeyCallBackType* key_callback) {
    // platformKeyCallback = key_callback != nullptr ? key_callback : platformKeyCallback;
}

void platformGLFWCMDLog(const char* message) {
    std::cerr << message << std::endl;
}

// Platform services -----------------------------------------------------------------
f64 platformGetTime() {
    return glfwGetTime();
}

f64 platformMeasureTimeElapsed(bool reset) {
    static f64 s_last_time = glfwGetTime();
    if (reset) { s_last_time = glfwGetTime(); }
    s_last_time = reset ? glfwGetTime() : s_last_time;
    return glfwGetTime() - s_last_time;
}

void platformDisableMouseCursor(void* platformWindow) {
    static GLFWwindow* window = nullptr;
    if (platformWindow) {
        window = (GLFWwindow*) platformWindow;
    } 
    if (window)  {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void platformEnableMouseCursor(void* platformWindow) {
    static GLFWwindow* window = nullptr;
    if (platformWindow) {
        window = (GLFWwindow*) platformWindow;
    } 
    if (window) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}