#ifndef MOLLY_UTIL_H
#define MOLLY_UTIL_H

#ifdef MOLLY_DEBUG
#ifdef _WIN32
#define DEBUG_BREAK __debugbreak();
#else
#define DEBUG_BREAK
#endif // _WIN32

#include <sstream>
#define GL_QUERY_ERROR(glFunction)                                                          \
    glFunction                                                                              \
    for (unsigned int err__ = glGetError(); err__ != GL_NO_ERROR; err__ = glGetError()) {   \
        std::ostringstream oss;                                                             \
        oss << "ERROR::OPENGL: 0x" << std::hex << err__ << std::dec                         \
            << " in file: " << __FILE__ << " at line: " << __LINE__;                        \
        utils::cmdlog(oss.str());                                                           \
        DEBUG_BREAK                                                                         \
    }

#define MOLLY_ASSERT(expr) \
    if (!expr) {*(int *)0 = 0;}
#else
#define GL_QUERY_ERROR(glFunction) \
    glFunction
#define MOLLY_ASSERT(expr)
#endif // MOLLY_DEBUG

#include <string>

#include "molly_math.h"

namespace utils 
{
struct ImageData {
    std::string name;
    unsigned char* data;
    int width;
    int height;
    int channel_count;
    int channel_size;
};

/* 
    Should you be in utils.h???
    If not the where should you be
    /// TODO: remove this out from utils
*/
enum class eMovement {
    kForward,
    kBackward,
    kLeft,
    kRight
};

void cmdlog(const std::string& message);
void print_GL_info();
std::string load_text_file(const char* path);
ImageData load_image_file(const std::string& path, bool flip = true);
void free_image_data(ImageData*);
std::string repeat(const std::string& str, int n);
std::string resolve_path(const std::string& path);
std::string toupper(const std::string& str);
std::string tolower(const std::string& str);
unsigned int load_image_to_opengl(ImageData&,unsigned int);

} // namespace molly

#endif //MOLLY_UTIL_H