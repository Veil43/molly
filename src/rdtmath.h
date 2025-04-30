#ifndef RENDERTOY_MATH_H
#define RENDERTOY_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
};

#endif // RENDERTOY_MATH_H