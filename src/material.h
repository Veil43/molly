#ifndef MOLLY_MATERIAL_H
#define MOLLY_MATERIAL_H

#include "types.h"

#define MOLLY_DIFFUSE_MAP_TEXTURE_UNIT            (0)
#define MOLLY_SPECULAR_MAP_TEXTURE_UNIT           (1)
#define MOLLY_METALLIC_ROUGHNESS_MAP_TEXTURE_UNIT (2)
#define MOLLY_NORMAL_MAP_TEXTURE_UNIT             (3)


struct BasicMaterial {
    u32 diffuse_map;
    u32 specular_map;
    f32 shininess;
};

#endif