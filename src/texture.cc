#define MOLLY_HAS_GL

#include "texture.h"
#include <glad/glad.h>

#include "utils.h"

Texture::Texture() noexcept 
    : m_id{0}
{}

/*
    Potentially add filtering mode selection?        
*/
Texture::Texture(const char* path) {
    GL_QUERY_ERROR(glGenTextures(1, &this->m_id);)
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, this->m_id);)

    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);)

    molly::ImageData image = molly::loadImageFile(path);
    u32 format = GL_RGB + (image.channel_count % 3);

    if (image.data) {
        GL_QUERY_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data);)
        GL_QUERY_ERROR(glGenerateMipmap(GL_TEXTURE_2D);)
    }

    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, 0);)
    molly::freeImageData(&image);
}

Texture::Texture(Texture&& other) noexcept  {
    if (this != &other) {
        this->m_id = other.m_id;
        other.m_id = 0;
    }
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        this->m_id = other.m_id;
        other.m_id = 0;
    }
    return *this;
}

Texture::~Texture() {
    GL_QUERY_ERROR(glDeleteTextures(1, &this->m_id);)
}

void Texture::bind(u32 unit) const noexcept {
    GL_QUERY_ERROR(glActiveTexture(GL_TEXTURE0 + unit);)
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, this->m_id);)
}

void Texture::unbind() const noexcept {
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, 0);)
}