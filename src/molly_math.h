#ifndef MOLLY_MATH_H
#define MOLLY_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

inline float molly_max_comp(const glm::vec3& vec) {
    return std::max(std::max(vec.x, vec.y), vec.z);
}

#endif // MOLLY_MATH_H