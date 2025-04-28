#ifndef RENDERTOY_SHADER_H
#define RENDERTOY_SHADER_H
#include "types.h"
#include <string>
#include "rdtmath.h"

#include "texture.h"

class Shader {
public:
    u32 m_id;

    Shader(const Shader&) =delete;
    Shader& operator=(const Shader&) =delete;

    Shader() noexcept;
    Shader(const char* vertex_source, const char* fragment_source);

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    ~Shader();

    void setInt(const std::string& name, i32 value) const ;
    void setFloat(const std::string& name, f32 value) const ;
    void setBool(const std::string& name, i32 value) const ;

    void setMat4f(const std::string& name, const glm::mat4 value) const ;
    void setVec3f(const std::string& name, const glm::vec3 value) const ;

    void bind() const;
    void unbind() const;
};

#endif // RASTERTOY_SHADER_H