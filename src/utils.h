#ifndef RENDERTOY_UTIL_H
#define RENDERTOY_UTIL_H

#ifdef RENDERTOY_DEBUG
#define GL_QUERY_ERROR(glFunction)                                                          \
    glFunction                                                                              \
    for (unsigned int err__ = glGetError(); err__ != GL_NO_ERROR; err__ = glGetError()) {   \
        std::ostringstream oss;                                                             \
        oss << "ERROR::OPENGL: 0x" << std::hex << err__ << std::dec                         \
            << " in file: " << __FILE__ << " at line: " << __LINE__;                        \
        rdt::log(oss.str());                                                                \
    }

#define RDT_ASSERT(expr) \
    if (!expr) *((void*)(0))
#else
#define GL_QUERY_ERROR(glFunction) \
    glFunction
#define RDT_ASSERT(expr)
#endif




#include <string>
#include <sstream>
#include "glfw_utils.h"
namespace rdt {
    struct ImageData {
        unsigned char* data;
        int width;
        int height;
        int channel_count;
    };

    /* 
        Should you be in utils.h???
        If not the where should you be
    */
    enum class eMovement {
        kForward,
        kBackward,
        kLeft,
        kRight
    };

    std::string loadTextFile(const char* path);
    void log(const std::string& message);
    void printGLInfo();
    ImageData loadImageFile(const char* path);
    void freeImageData(ImageData*);

} // namespace

#endif //RENDERTOY_UTIL_H
