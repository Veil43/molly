#ifndef MOLLY_CONFIG_H
#define MOLLY_CONFIG_H

#include <GLFW/glfw3.h>
#include <string>

static const int kOpenGLMajorVersion = 3;
static const int kOpenGLMinorVersion = 3;
static const int kOpenGLProfile = GLFW_OPENGL_CORE_PROFILE;

static const std::string kWindowTitle = "Molly with OpenGL";
static const float kWindowWidth = 800.0f;
static const float kWindowHeight = 600.0f;

#endif // MOLLY_CONFIG_H