#ifndef RENDERTOY_GLFW_PLATFORM_H
#define RENDERTOY_GLFW_PLATFORM_H

// Here we have the functions for the platform layer abstraction for all os-specific operations.
// As the name suggests we will not be implementing all oses ourselves and instead rely on glfw.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>

#include "types.h"

#define GLFW_DECL_KEY_CALLBACK(name) void name(GLFWwindow* window, int key, int scancode, int action, int mods)
typedef GLFW_DECL_KEY_CALLBACK(GLFWKeyCallBackType);

#define GLFW_DECL_FRAMEBUFFER_SIZE_CALLBACK(name) void name(GLFWwindow* window, int width, int height)
typedef GLFW_DECL_FRAMEBUFFER_SIZE_CALLBACK(glfw_frambuffer_callback_type);

#define GLFW_DECL_SCROLL_CALLBACK(name) void name(GLFWwindow* window, double xoffset, double yoffset)
typedef GLFW_DECL_SCROLL_CALLBACK(GLFWScrollCallbackType);

// We have windowing logic and much much more to come later...
struct GLFWPlatformWindow {
    std::string window_title;
    GLFWwindow* window_handle;
    i32 window_width;
    i32 window_height;
};

GLFWPlatformWindow platformCreateGLFWWindow();
void platformRunGLFWWindowLoop(GLFWPlatformWindow);
void platformCloseGLFWWindow(GLFWPlatformWindow);
std::string platformGetGLFWError(const std::string&);
void platformGLFWCMDLog(const char* message);
/*
    Deprecated: does nothing
*/
void platformSetGLFWKeyCallback(GLFWKeyCallBackType* key_callback);

// Platform Services API
#include "platform_services.h"

#endif // RENDERTOY_GLFW_PLATFORM_H