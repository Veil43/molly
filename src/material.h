#ifndef RENDERTOY_MATERIAL_H
#define RENDERTOY_MATERIAL_H

#include "types.h"

#define RDT_DIFFUSE_MAP_TEXTURE_UNIT            (0)
#define RDT_SPECULAR_MAP_TEXTURE_UNIT           (1)
#define RDT_METALLIC_ROUGHNESS_MAP_TEXTURE_UNIT (2)
#define RDT_NORMAL_MAP_TEXTURE_UNIT             (3)


struct BasicMaterial {
    u32 diffuse_map;
    u32 specular_map;
    f32 shininess;
};

#endif