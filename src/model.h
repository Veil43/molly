#ifndef RENDERTO_MODEL_H
#define RENDERTO_MODEL_H

#include <vector>
#include "mesh.h"

class Model {
    std::vector<StaticMesh> m_meshes;

    Model() =default;

    void Draw() const;
};

#endif // RENDERTO_MODEL_H