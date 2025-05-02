#ifndef MOLLY_LIGHT_H
#define MOLLY_LIGHT_H

#include "types.h"
#include "molly_math.h"

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

#endif // MOLLY_LIGHT_H