#ifndef MOLLY_SHADER_H
#define MOLLY_SHADER_H
#include "types.h"
#include "molly_math.h"
#include <string>

class Shader {
public:
    u32 m_id;

    Shader() noexcept;
    Shader(const std::string& vertex_source, const std::string& fragment_source);

    void set_int(const std::string& name, i32 value) const ;
    void set_float(const std::string& name, f32 value) const ;
    void set_bool(const std::string& name, i32 value) const ;

    void set_mat4f(const std::string& name, const glm::mat4 value) const ;
    void set_vec3f(const std::string& name, const glm::vec3 value) const ;
    void set_vec4f(const std::string& name, const glm::vec4 value) const ;

    void bind() const;
    void unbind() const;
};

Shader request_shader_of_name(const std::string& shader_name);
void   register_shader_with_name(const std::string& shader_name, Shader shader);

#endif // RASTERTOY_SHADER_H