#include "glfw_platform.h"

int main() {
    // Our new abstraction will turn our code into something close to this:
    GLFWPlatformWindow platform_window = platformCreateWindow();
    platformRunWindowLoop(platform_window);
    platformCloseWindow(platform_window);

    return 0;
}