#define MOLLY_HAS_GL
#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#undef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_WRITE_IMPLEMENTATION

#define TINYGLTF_IMPLEMENTATION
#include <glad/glad.h>

#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <filesystem>

#include "types.h"

std::string molly::load_text_file(const char* path) {
    std::ifstream file(path);

    if (!file) {
        std::cerr << "UTIL::IO: Could not open file: " << path << std::endl;
        return "";
    }

    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

void molly::log(const std::string& message) {
    std::cerr << message << std::endl;
}

void molly::print_GL_info() {
    GL_QUERY_ERROR(const char* version = (char*)glGetString(GL_VERSION);)
    GL_QUERY_ERROR(const char* vendor = (char*)glGetString(GL_VENDOR);)
    GL_QUERY_ERROR(const char* renderer = (char*)glGetString(GL_RENDERER);)
    GL_QUERY_ERROR(const char* glsl_version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);)
    molly::log(
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

    molly::log("[OpenGL::LIMITS]: Max Texture Units: " + std::to_string(max_texture_units));
    molly::log("[OpenGL::LIMITS]: Max Texture Size: " + std::to_string(max_texture_size) + "x" + std::to_string(max_texture_size) + " Pixels");
    molly::log("[OpenGL::LIMITS]: Max Draw Buffers: " + std::to_string(max_draw_buffers) + " (Like color/depth/stencil buffers)");
    molly::log("[OpenGL::LIMITS]: Max Vertex Attributes: " + std::to_string(max_vertex_attributes));
    molly::log("[OpenGL::LIMITS]: Max Uniforms: " + std::to_string(max_uniforms));

}

molly::ImageData molly::load_image_file(const char* path, bool flip) {
    molly::ImageData result = {};
    int x = 0;
    int y = 0;
    int c = 0;
    
    if (flip) {
        stbi_set_flip_vertically_on_load(1);
    }

    unsigned char* data = stbi_load(path, &x, &y, &c, 0);
    if (!data) {
        molly::log(std::string("ERROR::IO::STB_IMAGE: Could not load file with path <") + path + ">");
    } else {

        result.data = data;
        result.width = x;
        result.height = y;
        result.channel_count = c;
    }
    return std::move(result);
}

void molly::free_image_data(ImageData* img) {
    if (!img || !img->data)
        return;
 
    stbi_image_free(img->data);
    img->data = nullptr;
    img->width = 0;
    img->height = 0;
    img->channel_count = 0;
}

std::string molly::repeat(const std::string& str, int n) {
    std::string tmp = str;
    while (n>1) {
        tmp+=str;
        n--;
    }
    return tmp;
}

std::string molly::resolve_path(const std::string& path) {
    namespace fs = std::filesystem;
    std::cout << path << std::endl;
    std::string output_path = path;
    for (int i = 0; i < 7; i++) {
        if (fs::exists(output_path)) return output_path;
        output_path = repeat("../", i) + path;
    }

    return output_path;
}

std::string molly::toupper(const std::string& str) {
    std::string output_string = str;
    std::transform(output_string.begin(), output_string.end(), output_string.begin(), 
                   [](u8 c){ return std::toupper(c); });
    return output_string;
}
 std::string molly::tolower(const std::string& str) {
    std::string output_string = str;
    std::transform(output_string.begin(), output_string.end(), output_string.begin(), 
                   [](u8 c){ return std::tolower(c); });
    return output_string;
 }

unsigned int molly::load_image_to_opengl(ImageData& image, unsigned int texture_unit) {
    if (!image.data) {
        return 0;
    }
    unsigned int texture;
    unsigned int internal_format = GL_RGB8;
    if (image.channel_count == 4) {
        internal_format = GL_RGBA8;
    }

    GL_QUERY_ERROR(glGenTextures(1, &texture);)
    GL_QUERY_ERROR(glActiveTexture(GL_TEXTURE0 + texture_unit);)
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, texture);)

    GL_QUERY_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);)
    GL_QUERY_ERROR(glGenerateMipmap(GL_TEXTURE_2D);)

    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);)

    return texture;
}