#ifndef RENDERTOY_SCENE_H
#define RENDERTOY_SCENE_H

#include <vector>
#include "types.h"

struct Node {
    i32 id;
};

struct Scene {
    std::vector<i32> m_nodes;
};

#endif // RENDERTOY_SCENE_H