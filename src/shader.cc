#define RENDERTOY_HAS_GL
#include <glad/glad.h>
#include <string>
#include "shader.h"
#include "utils.h"

/*
    @brief OpenGL util to compile shaders.
    @param shader_id
    The OpenGL handle for the shader to compile
    @param shader_type
    The type of shader you wish to compile
    Expects:
    `0` for `VERTEX`
    `1` for `FRAGMENT`
    @param shader_source
    The string containing the source code of the shader to be compiled.
*/
int compileShader(u32 shader_id, u32 shader_type, const char* shader_source) {
    GL_QUERY_ERROR(glShaderSource(shader_id, 1, &shader_source, NULL);)
    GL_QUERY_ERROR(glCompileShader(shader_id);)
    int success;
    char info_log[512];
    GL_QUERY_ERROR(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);)
    if (!success) {
        GL_QUERY_ERROR(glGetShaderInfoLog(shader_id, 512, NULL, info_log);)
        const char* type_string = shader_type ? "FRAGMENT::" : "VERTEX::";
        rdt::log(std::string("[ERROR]::SHADER::") + type_string + "COMPILATION_FAILED\n" + info_log);
        return 0;
    }
    return 1;
}

Shader::Shader() noexcept 
    : m_id{0}
{}

Shader::Shader(const char* vpath, const char* fpath) {
    const std::string vsource = rdt::loadTextFile(vpath);
    const std::string fsource = rdt::loadTextFile(fpath);
    GL_QUERY_ERROR(u32 vshader = glCreateShader(GL_VERTEX_SHADER);)
    GL_QUERY_ERROR(u32 fshader = glCreateShader(GL_FRAGMENT_SHADER);)
    compileShader(vshader, 0, vsource.c_str());
    compileShader(fshader, 1, fsource.c_str());
    GL_QUERY_ERROR(this->m_id = glCreateProgram();)
    GL_QUERY_ERROR(glAttachShader(this->m_id, vshader);)
    GL_QUERY_ERROR(glAttachShader(this->m_id, fshader);)
    GL_QUERY_ERROR(glLinkProgram(this->m_id);)
    int success;
    char info_log[512];
    GL_QUERY_ERROR(glGetProgramiv(this->m_id, GL_LINK_STATUS, &success);)
    if (!success) {
        GL_QUERY_ERROR(glGetProgramInfoLog(this->m_id, 512, NULL, info_log);)
        GL_QUERY_ERROR(rdt::log(std::string("[ERROR]::SHADER::") + "LINKING_FAILED\n" + info_log);)
    }
    GL_QUERY_ERROR(glDeleteShader(vshader);)
    GL_QUERY_ERROR(glDeleteShader(fshader);)
}

Shader::Shader(Shader&& other) noexcept {
    if (this != &other) {
        this->m_id = other.m_id;
        other.m_id = 0;
    }
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        this->m_id = other.m_id;
        other.m_id = 0;
    }
    return *this;
}

Shader::~Shader() {
    if (m_id) {
        GL_QUERY_ERROR(glDeleteProgram(m_id);)
        m_id = 0;
    }
}

void Shader::setInt(const std::string& name, i32 value) const {
    GL_QUERY_ERROR(i32 location = glGetUniformLocation(this->m_id, name.c_str());)
    GL_QUERY_ERROR(glUniform1i(location, value);)
}

void Shader::setBool(const std::string& name, i32 value) const {
    this->setInt(name, value);
}

void Shader::setFloat(const std::string& name, f32 value) const {
    GL_QUERY_ERROR(i32 location = glGetUniformLocation(this->m_id, name.c_str());)
    GL_QUERY_ERROR(glUniform1f(location, value);)
}

void Shader::setMat4f(const std::string& name, const glm::mat4 value) const {
    GL_QUERY_ERROR(i32 location = glGetUniformLocation(this->m_id, name.c_str());)
    GL_QUERY_ERROR(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));)
}

void Shader::setVec3f(const std::string& name, const glm::vec3 value) const  {
    GL_QUERY_ERROR(i32 location = glGetUniformLocation(this->m_id, name.c_str());)
    GL_QUERY_ERROR(glUniform3fv(location, 1, glm::value_ptr(value));)
}

void Shader::setVec4f(const std::string& name, const glm::vec4 value) const  {
    GL_QUERY_ERROR(i32 location = glGetUniformLocation(this->m_id, name.c_str());)
    GL_QUERY_ERROR(glUniform3fv(location, 1, glm::value_ptr(value));)
}


void Shader::bind() const {
    GL_QUERY_ERROR(glUseProgram(m_id);)
}

void Shader::unbind() const {
    GL_QUERY_ERROR(glUseProgram(0);)
}