#ifndef MOLLY_MATH_H
#define MOLLY_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

class Transform{
public:
    glm::vec3 scale = glm::vec3(1.0);
    glm::vec3 translation = glm::vec3(0.0);
    glm::quat rotation = glm::quat(1.0, 0.0, 0.0, 0.0);

    glm::mat4 to_mat4() const {
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);
        glm::mat4 rotation_mat = glm::mat4_cast(rotation);
        glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), translation);
        return translation_mat * rotation_mat * scale_mat;;
    };
};

inline float molly_max_comp(const glm::vec3& vec) {
    return std::max(std::max(vec.x, vec.y), vec.z);
}

inline glm::vec3 molly_get_translation(const glm::mat4& matrix) {
    float x = matrix[3][0];
    float y = matrix[3][1];
    float z = matrix[3][2];

    return glm::vec3(x, y, z);
}

inline Transform compose(const Transform& parent, const Transform& child) {
    Transform output_transform = {};
    output_transform.scale = parent.scale * child.scale;
    output_transform.rotation = parent.rotation * child.rotation;
    output_transform.translation = parent.translation + parent.rotation * (parent.scale * child.translation);
    return output_transform;
}

#endif // MOLLY_MATH_H