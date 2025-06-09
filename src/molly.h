#include "model.h"
    
namespace cube
{
        
static float raw_positions[] = {
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
   0.5f,  0.5f, -0.5f,
   0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f,
   0.5f, -0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,
  -0.5f, -0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,
   0.5f,  0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   0.5f, -0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,
  -0.5f, -0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   0.5f, -0.5f,  0.5f,
   0.5f, -0.5f,  0.5f,
  -0.5f, -0.5f,  0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f,
   0.5f,  0.5f, -0.5f,
   0.5f,  0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f, -0.5f,
};
static float raw_normals[] = {
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
};
static float raw_tex_coords[] = {
0.0f, 0.0f,
1.0f, 0.0f,
1.0f, 1.0f,
1.0f, 1.0f,
0.0f, 1.0f,
0.0f, 0.0f,
0.0f, 0.0f,
1.0f, 0.0f,
1.0f, 1.0f,
1.0f, 1.0f,
0.0f, 1.0f,
0.0f, 0.0f,
1.0f, 0.0f,
1.0f, 1.0f,
0.0f, 1.0f,
0.0f, 1.0f,
0.0f, 0.0f,
1.0f, 0.0f,
1.0f, 0.0f,
1.0f, 1.0f,
0.0f, 1.0f,
0.0f, 1.0f,
0.0f, 0.0f,
1.0f, 0.0f,
0.0f, 1.0f,
1.0f, 1.0f,
1.0f, 0.0f,
1.0f, 0.0f,
0.0f, 0.0f,
0.0f, 1.0f,
0.0f, 1.0f,
1.0f, 1.0f,
1.0f, 0.0f,
1.0f, 0.0f,
0.0f, 0.0f,
0.0f, 1.0f
};

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
    1.0f,
};

inline gltf::ModelData get_cube_model(int material_index) {
    unsigned char* pos_data = reinterpret_cast<unsigned char*>(raw_positions);
    unsigned char* nor_data = reinterpret_cast<unsigned char*>(raw_normals);
    unsigned char* tex_data = reinterpret_cast<unsigned char*>(raw_tex_coords);

    size_t psize = sizeof(raw_positions);
    size_t nsize = sizeof(raw_normals);
    size_t tsize = sizeof(raw_tex_coords);
    std::vector<unsigned char> positions{};
    std::vector<unsigned char> normals{};
    std::vector<unsigned char> tex_coords{};
    positions.reserve(psize);
    normals.reserve(nsize);
    tex_coords.reserve(tsize);

    for (size_t i = 0; i < psize; i++) {
        positions.push_back(pos_data[i]);
    }
    for (size_t i = 0; i < nsize; i++) {
        normals.push_back(nor_data[i]);
    }
    for (size_t i = 0; i < tsize; i++) {
        tex_coords.push_back(tex_data[i]);
    }

    gltf::MeshData mesh = {};
    mesh.name = std::string("cube_body");
    mesh.transform = glm::mat4(1.0f);
    // mesh.position = glm::vec3(0.0);
    // mesh.scale = glm::vec3(0.0);
    // mesh.rotation = glm::quat();
    mesh.pos_data = std::move(positions);
    mesh.nor_data = std::move(normals);
    mesh.tex_data_0 = std::move(tex_coords);
    mesh.indices_data = {};
    mesh.material_index = material_index;
    mesh.indices_type = gltf::eBasicType::kUnsignedInt;
    mesh.texture_type_0 = gltf::eBasicType::kFloat;
    
    std::vector<gltf::MeshData> meshes;
    meshes.push_back(mesh);
    gltf::ModelData cube {
        std::string("cube"),
        std::move(meshes)
    };
    return cube;
}

} // namespace cube