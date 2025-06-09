#ifndef MOLLY_MATH_H
#define MOLLY_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

inline float molly_max_comp(const glm::vec3& vec) {
    return std::max(std::max(vec.x, vec.y), vec.z);
}

inline glm::vec3 molly_get_translation(const glm::mat4& matrix) {
    float x = matrix[3][0];
    float y = matrix[3][1];
    float z = matrix[3][2];

    return glm::vec3(x, y, z);
}

#endif // MOLLY_MATH_H