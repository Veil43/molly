#ifndef RENDERTOY_GLFW_PLATFORM_H
#define RENDERTOY_GLFW_PLATFORM_H
// Here we have the functions for the platform layer abstraction for all os-specific operations.
// As the name suggests we will not be implementing all oses ourselves and instead rely on glfw.

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>

#include "types.h"
// We have windowing logic and much much more to come later...

#define PLATFROM_DECL_KEY_CALLBACK(name) void name(GLFWwindow* window, int key, int scancode, int action, int mods)
typedef PLATFROM_DECL_KEY_CALLBACK(GLFWKeyCallBackType);

#define PLATFORM_DECL_FRAMEBUFFER_SIZE_CALLBACK(name) void name(GLFWwindow* window, int width, int height)
typedef PLATFORM_DECL_FRAMEBUFFER_SIZE_CALLBACK(glfw_frambuffer_callback_type);

struct GLFWPlatformWindow {
    std::string window_title;
    GLFWwindow* window_handle;
    i32 window_width;
    i32 window_height;
};

GLFWPlatformWindow platformCreateWindow();
void platformRunWindowLoop(GLFWPlatformWindow);
void platformCloseWindow(GLFWPlatformWindow);
void PlatformSetCallbacks();
std::string platformGetError(const std::string&);
void platformCMDLog(const char* message);
void platformSetCallback(GLFWKeyCallBackType* key_callback);

#endif // RENDERTOY_GLFW_PLATFORM_H