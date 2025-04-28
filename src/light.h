#ifndef RENDERTOY_LIGHT_H
#define RENDERTOY_LIGHT_H

#include "types.h"
#include "rdtmath.h"

struct PointLight {
    glm::vec3 position;
    glm::vec3 attenuation;
    
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular; 
};

struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 attenuation;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    glm::vec3 position;     // 12 bytes size 12 bytes align
    glm::vec3 direction;
    glm::vec3 attenuation;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    f32 cutoff;
    f32 outer_cutoff;
};

#endif // RENDERTOY_LIGHT_H