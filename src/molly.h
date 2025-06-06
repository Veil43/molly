static float gs_box[] = {
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

     -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

     -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f
};

#include "model.h"

namespace box
{

static gltf::TextureInfo texture = {
    std::string("white_texture"),
    std::string("assets/textures/whiteP6.ppm")
};

inline gltf::MaterialInfo material = {
    std::string("cube_material"),
    texture,
    gltf::TextureInfo{},
    gltf::TextureInfo{},
    glm::vec4(1.0),
    1.0f,
    1.0f,
    1.0f
};

inline gltf::ModelData get_cube_model(int material_index) {
    unsigned char* data = reinterpret_cast<unsigned char*>(gs_box);
    size_t size = sizeof(gs_box);

    std::vector<unsigned char> vertices{};
    vertices.reserve(size);

    for (size_t i = 0; i < size; i++) {
        vertices.push_back(data[i]);
    }

    gltf::MeshData mesh {
        std::string("cube_body"),
        glm::mat4(1.0f),
        std::move(vertices),
        {},
        {},
        {},
        material_index,
        gltf::eBasicType::kUnsignedInt,
        gltf::eBasicType::kFloat
    };

    std::vector<gltf::MeshData> meshes;
    meshes.push_back(mesh);
    gltf::ModelData cube {
        std::string("cube"),
        std::move(meshes)
    };
    return cube;
}

}