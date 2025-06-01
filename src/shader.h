#ifndef MOLLY_SHADER_H
#define MOLLY_SHADER_H
#include "types.h"
#include "molly_math.h"
#include <string>

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

    void set_int(const std::string& name, i32 value) const ;
    void set_float(const std::string& name, f32 value) const ;
    void set_bool(const std::string& name, i32 value) const ;

    void set_mat4f(const std::string& name, const glm::mat4 value) const ;
    void set_vec3f(const std::string& name, const glm::vec3 value) const ;
    void set_vec4f(const std::string& name, const glm::vec4 value) const ;

    void bind() const;
    void unbind() const;
};

#endif // RASTERTOY_SHADER_H