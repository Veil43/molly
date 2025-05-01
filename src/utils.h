#ifndef RENDERTOY_UTIL_H
#define RENDERTOY_UTIL_H

#ifdef RENDERTOY_DEBUG
#ifdef _WIN32
#define DEBUG_BREAK __debugbreak();
#else
#define DEBUG_BREAK
#endif

#define GL_QUERY_ERROR(glFunction)                                                          \
    glFunction                                                                              \
    for (unsigned int err__ = glGetError(); err__ != GL_NO_ERROR; err__ = glGetError()) {   \
        std::ostringstream oss;                                                             \
        oss << "ERROR::OPENGL: 0x" << std::hex << err__ << std::dec                         \
            << " in file: " << __FILE__ << " at line: " << __LINE__;                        \
        rdt::log(oss.str());                                                                \
        DEBUG_BREAK                                                                         \
    }

#define RDT_ASSERT(expr) \
    if (!expr) {*(int *)0 = 0;}
#else
#define GL_QUERY_ERROR(glFunction) \
    glFunction
#define RDT_ASSERT(expr)
#endif

#include <string>
#include <sstream>
#include <vector>

#include "glfw_utils.h"
#include "rdtmath.h"

namespace rdt {
    struct ImageData {
        unsigned char* data;
        int width;
        int height;
        int channel_count;
        int channel_size;
    };

    struct MeshData {
        glm::mat4 world_transform;
        float* vertex_data;
        unsigned int* index_data;
        int vertex_count;
        int index_count;
        // unsigned int draw_mode;
        unsigned int material_index;
    };

    struct TextureConfig {
        unsigned int min_filter;
        unsigned int mag_filter;
        unsigned int wrap_s;
        unsigned int wrap_t;
    };

    struct TextureInfo {
        TextureConfig filter_wrap_config;
        std::string image_uri;
        unsigned int image_index;
    };
    
    struct MaterialData {
        TextureInfo base_color;
        TextureInfo metallic_roughness;
        TextureInfo normal;
        // Currently not supported
        TextureInfo emissive; 
        // Currently not supported
        TextureInfo occlusion;
        glm::vec4 base_color_factor;
        float metallic_factor;
        float roughness_factor;
    };

    struct ModelData {
        std::vector<MeshData> meshes;
        std::vector<MaterialData> materials;
        std::vector<ImageData> images;
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

    enum eTextureConfigOptions {
        kTextureFilterNearest,
        kTextureFilterLinear,
        kTextureFilterNearestMipmapNearest,
        kTextureFilterLinearMipmapNearest,
        kTextureFilterNearestMipmapLinear,
        kTextureFilterLinearMipmapLinear,
        kTextureWrapRepeat,
        kTextureWrapClampToEdge,
        kTextureWrapMirroredRepeat,
        kError,
    };

    std::string loadTextFile(const char* path);
    void log(const std::string& message);
    void printGLInfo();
    ImageData loadImageFile(const char* path);
    void freeImageData(ImageData*);
    ModelData loadModel(const char* path);
    void freeModel(ModelData*);
    float* concatModelArrays(const MeshData& data);

} // namespace rdt

#endif //RENDERTOY_UTIL_H
