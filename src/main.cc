#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "molly_services.h"

#define WINDOW_WIDTH 800.0
#define WINDOW_HEIGHT 600.0
#define WINDOW_TITLE "OpenGL with Molly"

static PlatformInput 
process_input(GLFWwindow* w, PlatformInput old_input);
static void 
glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

static GLFWwindow* sg_window_handle = nullptr;
int main() {

    // Initialize windowing
    if (!glfwInit()) {
        std::cerr << "Error: could not initialize glfw3!" << std::endl;
        exit(1);
    }

    // configure OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // configure glfw window
    const float c_window_width = WINDOW_WIDTH;
    const float c_window_height = WINDOW_HEIGHT;
    const char* c_window_title = WINDOW_TITLE;
    GLFWwindow* window_handle = glfwCreateWindow(
        c_window_width,
        c_window_height,
        c_window_title,
        nullptr, nullptr
    );

    if (!window_handle) {
        std::cerr << "Error: could not create a window with glfw3!" << std::endl;
        glfwTerminate();
        exit(1);
    }
    sg_window_handle = window_handle;
    glfwMakeContextCurrent(window_handle);

    if (!gladLoadGL()) {
        glfwDestroyWindow(window_handle);
        glfwTerminate();
        exit(1);
    }

    // glfwSetKeyCallback(window_handle);
    // glfwSetFramebufferSizeCallback(window_handle);
    glfwSetScrollCallback(window_handle, glfw_scroll_callback);

    // Call application startup code
    molly_on_startup_call(c_window_width/c_window_height);

    PlatformInput old_input = {};
    while(!glfwWindowShouldClose(window_handle)) {
        glfwPollEvents();
        
        // Call application looping code ----------
        PlatformInput new_input = process_input(window_handle, old_input);
        old_input = new_input;
        molly_render_loop(new_input);

        glfwSwapBuffers(window_handle);
    }
   
    glfwDestroyWindow(window_handle);
    glfwTerminate();
    return 0;
}

static PlatformInput 
process_input(GLFWwindow* window_handle, PlatformInput old_input) {
    static bool first_input = true;

    PlatformInput input = {};
    input.w_key.is_down = (glfwGetKey(window_handle, GLFW_KEY_W) == GLFW_PRESS);
    input.w_key.was_down = old_input.w_key.is_down;

    input.a_key.is_down = (glfwGetKey(window_handle, GLFW_KEY_A) == GLFW_PRESS);
    input.a_key.was_down = old_input.a_key.is_down;

    input.s_key.is_down = (glfwGetKey(window_handle, GLFW_KEY_S) == GLFW_PRESS);
    input.s_key.was_down = old_input.s_key.is_down;

    input.d_key.is_down = (glfwGetKey(window_handle, GLFW_KEY_D) == GLFW_PRESS);
    input.d_key.was_down = old_input.d_key.is_down;

    input.esc_key.is_down = (glfwGetKey(window_handle, GLFW_KEY_ESCAPE) == GLFW_PRESS);
    input.esc_key.was_down = old_input.esc_key.is_down;

    if (first_input) {
        old_input.mouse_x = WINDOW_WIDTH/2;
        old_input.mouse_y = WINDOW_HEIGHT/2;
        first_input = false;
    }

    f64 xpos;
    f64 ypos;
    glfwGetCursorPos(window_handle, &xpos, &ypos);

    input.mouse_x = static_cast<f32>(xpos);
    input.mouse_prevx = old_input.mouse_x;
    input.mouse_y = static_cast<f32>(ypos);
    input.mouse_prevy = old_input.mouse_y;

    return input;
}

static void
glfw_scroll_callback(GLFWwindow* window_handle, double xoffset, double yoffset) {
    molly_mouse_scroll(static_cast<float>(yoffset));
}

// ------------------------------------------------------------------------------
// PLATFORM SERVICES
// ------------------------------------------------------------------------------
f64 platform_get_time() {
    return glfwGetTime();
}

/*
    Returns the time since the last call with resset == true
*/
f64 platform_measure_time_elapsed(bool reset) {
    static f64 s_last_time = 0.0;
    static bool s_init = false;

    const f64 now = glfwGetTime();

    if (reset || !s_init) {
        s_last_time = now;
        s_init = true;
        return 0.0;
    }

    const f64 delta = now - s_last_time;
    s_last_time = now;
    return delta;
}

void platform_disable_mouse_cursor() {
    if (sg_window_handle)  {
        glfwSetInputMode(sg_window_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void platform_enable_mouse_cursor(void* platformWindow) {
    if (sg_window_handle) {
        glfwSetInputMode(sg_window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void platform_request_quit() {
    glfwSetWindowShouldClose(sg_window_handle, GLFW_TRUE);   
}