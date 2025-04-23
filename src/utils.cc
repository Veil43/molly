#include "utils.h"
#define RENDERTOY_HAS_GL
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <sstream>
#include <fstream>
#include <iostream>

#include "types.h"

std::string rdt::loadTextFile(const char* path) {
    std::ifstream file(path);

    if (!file) {
        std::cerr << "UTIL::IO: Could not open file: " << path << std::endl;
        return "";
    }

    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

void rdt::log(const std::string& message) {
    std::cerr << message << std::endl;
}

void rdt::printGLInfo() {
    GL_QUERY_ERROR(const char* version = (char*)glGetString(GL_VERSION);)
    GL_QUERY_ERROR(const char* vendor = (char*)glGetString(GL_VENDOR);)
    GL_QUERY_ERROR(const char* renderer = (char*)glGetString(GL_RENDERER);)
    GL_QUERY_ERROR(const char* glsl_version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);)
    rdt::log(
        std::string(
        "================== OpenGL Information ===================\n") + 
        "VERSION: " + version + "\n"
        "VENDOR: " + vendor + "\n"
        "RENDERER: " + renderer + "\n"
        "GLSL VERSION: " + glsl_version + "\n"
    );

    i32 max_texture_units = 0;
    i32 max_texture_size = 0;
    i32 max_draw_buffers = 0; // like color buffers, depth buffers etc
    i32 max_vertex_attributes = 0;
    i32 max_uniforms = 0;
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);)
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);)
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);)
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attributes);)
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max_uniforms);)

    rdt::log("[OpenGL::LIMITS]: Max Texture Units: " + std::to_string(max_texture_units));
    rdt::log("[OpenGL::LIMITS]: Max Texture Size: " + std::to_string(max_texture_size) + "x" + std::to_string(max_texture_size) + " Pixels");
    rdt::log("[OpenGL::LIMITS]: Max Draw Buffers: " + std::to_string(max_draw_buffers) + " (Like color/depth/stencil buffers)");
    rdt::log("[OpenGL::LIMITS]: Max Vertex Attributes: " + std::to_string(max_vertex_attributes));
    rdt::log("[OpenGL::LIMITS]: Max Uniforms: " + std::to_string(max_uniforms));

}

rdt::ImageData rdt::loadImageFile(const char* path) {
    rdt::ImageData result = {};
    int x = 0;
    int y = 0;
    int c = 0;
    unsigned char* data = stbi_load(path, &x, &y, &c, 0);
    if (!data) {
        rdt::log(std::string("ERROR::IO::STB_IMAGE: Could not load file with path <") + path + ">");
    } else {

        result.data = data;
        result.width = x;
        result.height = y;
        result.channel_count = c;
    }
    return result;
}

void rdt::freeImageData(ImageData* img) {
    if (!img || !img->data)
        return;
 
    stbi_image_free(img->data);
    img->data = nullptr;
    img->width = 0;
    img->height = 0;
    img->channel_count = 0;
}